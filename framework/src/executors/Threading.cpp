// Copyright 2025 JesusTouchMe

#include "JesusVM/JesusVM.h"

#include "JesusVM/executors/Threading.h"
#include "JesusVM/executors/Thread.h"


#include <condition_variable>
#include <mutex>
#include <queue>

#undef VOID
#undef BOOL

namespace JesusVM::Threading {
    std::unordered_map<std::thread::id, std::unique_ptr<Thread>> threads;
    std::unordered_map<std::thread::id, std::unique_ptr<VThreadGroup>> vThreadGroups;

    std::queue<Function*> eventQueue;

    std::mutex mutex;
    std::condition_variable terminateCondition;

    u64 nonDaemonThreads = 0;

    template<class T>
    static void AttachThread(std::unique_ptr<T> thread) {
        if constexpr (std::is_same_v<T, Thread>) {
            threads[thread->getId()] = std::move(thread);
        } else if constexpr (std::is_same_v<T, VThreadGroup>) {
            vThreadGroups[thread->getId()] = std::move(thread);
        } else {
            static_assert(false, "Bad thread type");
        }

        if (!thread->isDaemon()) {
            nonDaemonThreads++;
        }
    }

    template<class T>
    static void DetachThread(T* thread) {
        thread->setState(ThreadState::TERMINATED);

        threads.erase(thread->getId());

        if (!thread->isDaemon()) {
            nonDaemonThreads--;
        }

        terminateCondition.notify_all();
    }

    static void ThreadEntry(Thread* thread) {
        thread->start();

        std::lock_guard<std::mutex> lock(mutex);
        DetachThread(thread);
    }

    void Init() {
        auto mainThread = std::make_unique<Thread>();
        mainThread->mId = std::this_thread::get_id();

        std::lock_guard<std::mutex> lock(mutex);

        AttachThread(std::move(mainThread));
        nonDaemonThreads--; // mainThread is NOT a daemon thread, but we don't increment nonDaemonThreads since it would cause the program to last forever (main thread is never detached)
    }

    static inline void LaunchThreadInternal(Function* function) {
        auto thread = std::make_unique<Thread>();
        Thread* tmpThread = thread.get();

        std::lock_guard<std::mutex> lock(mutex);

        std::thread nativeThread(ThreadEntry, thread.get());
        thread->mId = nativeThread.get_id();

        AttachThread(std::move(thread));
        nativeThread.detach();

        tmpThread->setFunction(function); // set this after everything so Thread::start will be waiting in case that gets reached before the thread isn't setup
    }

    void LaunchThread(Function* function) {
        if (function->getReturnType().getInternalType() != Type::VOID || !function->getArgumentTypes().empty()) {
            return; // only ()V functions can run on a thread
        }

        LaunchThreadInternal(function);
    }

    // Schedules a function to be run in a Thread. Whether it's added to the event queue or a new thread is born is also determined here
    static inline void ScheduleOS(Function* function) {
        //TODO: make these configurable
        constexpr u64 minThreadsForFilledEventQueue = 12;
        constexpr u64 minEventQueueSizeBeforeThreadLaunch = 4;
        constexpr u64 maxThreadsForQueuedFunctions = 24;
        constexpr u64 minThreads = 6;
        constexpr double eventQueueToThreadCountRatioThreshold = 2.0;
        constexpr u64 maxConcurrentThreads = 50;

        u64 threadCount = ThreadCount();

        if (threadCount >= maxConcurrentThreads) { // Not much to do in this situation
            eventQueue.push(function);
            return;
        }

        if (function->isNative()) { // Native functions are more likely to give birth to a new thread
            if (threadCount >= minThreadsForFilledEventQueue
                && eventQueue.size() < minEventQueueSizeBeforeThreadLaunch
                && threadCount < maxThreadsForQueuedFunctions) {
                eventQueue.push(function);
            } else {
                LaunchThreadInternal(function);
            }
            return;
        }

        if (threadCount < minThreads) {
            LaunchThreadInternal(function);
            return;
        } else if (eventQueue.empty() || function->getBytecodeSize() < 50) { // We don't like making a whole new thread just for a small function
            eventQueue.push(function);
            return;
        }

        if (threadCount < minThreadsForFilledEventQueue && eventQueue.size() > minEventQueueSizeBeforeThreadLaunch) {
            LaunchThreadInternal(function);
            return;
        }

        if (static_cast<double>(eventQueue.size()) * eventQueueToThreadCountRatioThreshold >= static_cast<double>(threadCount)) {
            LaunchThreadInternal(function);
        } else {
            eventQueue.push(function);
        }
    }

    static VThreadGroup* GetIdealVThreadGroup() {
        VThreadGroup* best = nullptr;
        for (const auto& [key, group] : vThreadGroups) {
            if (group->getThreadCount() <= 1)
                return group.get();

            if (best == nullptr || group->getThreadCount() < best->getThreadCount()) {
                best = group.get();
            }
        }

        return best;
    }

    void Schedule(Function* function) {
        constexpr u64 maxVThreads = 20;
        constexpr u64 maxConcurrentThreads = 50;

        if (function->getReturnType().getInternalType() != Type::VOID || !function->getArgumentTypes().empty()) {
            return; // only ()V functions can run on a thread
        }

        if (function->isNative()) {
            ScheduleOS(function);
            return;
        }

        u64 threadCount = ThreadCount();

        if (threadCount >= maxConcurrentThreads) {
            if (function->isPure() || function->getBytecodeSize() < 50) {
                VThreadGroup* group = GetIdealVThreadGroup();
                if (group->getThreadCount() < maxVThreads) {
                    group->runFunction(function);
                } else {
                    eventQueue.push(function);
                }
            }
        }

        if (function->isPure()) {
            if (vThreadGroups.size() > )
        }
    }

    Handle CurrentThread() {
        auto it = threads.find(std::this_thread::get_id());

        if (it != threads.end()) {
            return it->second.get();
        }

        auto it2 = vThreadGroups.find(std::this_thread::get_id());

        if (it2 != vThreadGroups.end()) {
            return it2->second.get();
        }

        return nullptr;
    }

    u64 ThreadCount() {
        return threads.size() + vThreadGroups.size();
    }

    void WaitForAllThreads() {
        std::unique_lock<std::mutex> lock(mutex);

        terminateCondition.wait(lock, [] {
            return nonDaemonThreads == 0;
        });
    }
}
