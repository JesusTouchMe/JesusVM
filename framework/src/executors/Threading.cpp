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
    std::mutex eventQueueMutex;
    std::condition_variable eventQueueCondition;

    std::mutex mutex;
    std::condition_variable terminateCondition;

    u64 nonDaemonThreads = 0;

    template<class T>
    static void AttachThread(std::unique_ptr<T> thread) {
        if (!thread->isDaemon()) {
            nonDaemonThreads++;
        }

        if constexpr (std::is_same_v<T, Thread>) {
            threads[thread->getId()] = std::move(thread);
        } else if constexpr (std::is_same_v<T, VThreadGroup>) {
            vThreadGroups[thread->getId()] = std::move(thread);
        } else {
            static_assert(false, "Bad thread type");
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
        using namespace std::chrono_literals;

        while (!thread->isInterrupted()) {
            thread->start();

            std::unique_lock<std::mutex> lock(eventQueueMutex);
            if (!eventQueueCondition.wait_for(lock, 1s, [thread] { return !eventQueue.empty() && !thread->isInterrupted(); })) {
                break;
            }

            Function* function = eventQueue.front();
            eventQueue.pop();
            thread->setFunction(function);
        }

        std::lock_guard<std::mutex> lock(mutex);
        DetachThread(thread);
    }

    static void VThreadGroupEntry(VThreadGroup* thread) {
        using namespace std::chrono_literals;

        while (!thread->isInterrupted()) {
            thread->start();

            // in the case that the group stops, try to get it running again with functions from the event queue

            std::unique_lock<std::mutex> lock(eventQueueMutex);
            if (!eventQueueCondition.wait_for(lock, 200ms, [thread] { return !eventQueue.empty() && !thread->isInterrupted(); })) {
                break;
            }

            Function* function = eventQueue.front();
            eventQueue.pop();
            thread->runFunction(function);
        }

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

    static void LaunchThreadInternal(Function* function) {
        auto thread = std::make_unique<Thread>();
        Thread* tmpThread = thread.get();


        std::thread nativeThread(ThreadEntry, tmpThread);
        thread->mId = nativeThread.get_id();

        {
            std::lock_guard<std::mutex> lock(mutex);
            AttachThread(std::move(thread));
        }

        nativeThread.detach();

        tmpThread->setFunction(function); // set this after everything so Thread::start will be waiting in case that gets reached before the thread isn't setup
    }

    static VThreadGroup* LaunchVThreadGroup() {
        auto thread = std::make_unique<VThreadGroup>();
        VThreadGroup* tmpThread = thread.get();

        std::thread nativeThread(VThreadGroupEntry, tmpThread);
        thread->mId = nativeThread.get_id();

        {
            std::lock_guard<std::mutex> lock(mutex);
            AttachThread(std::move(thread));
        }

        nativeThread.detach();

        return tmpThread;
    }

    void LaunchThread(Function* function) {
        if (function->getReturnType().getInternalType() != Type::VOID || !function->getArgumentTypes().empty()) {
            return; // only ()V functions can run on a thread
        }

        LaunchThreadInternal(function);
    }
    
    static void EnqueueFunction(Function* function) {
        {
            std::lock_guard<std::mutex> lock(eventQueueMutex);
            eventQueue.push(function);
        }

        eventQueueCondition.notify_one();
    }

    // Schedules a function to be run in a Thread. Whether it's added to the event queue or a new thread is born is also determined here
    static inline void ScheduleOS(Function* function) {
        u64 threadCount = ThreadCount();

        if (threadCount >= opt::MAX_CONCURRENT_THREADS) {
            EnqueueFunction(function);
            return;
        }

        if (function->isNative()) {
            if (threadCount >= opt::MIN_THREADS_FOR_FILLED_EVENT_QUEUE &&
                eventQueue.size() < opt::MIN_EVENT_QUEUE_SIZE_BEFORE_THREAD_LAUNCH &&
                threadCount < opt::MAX_THREADS_FOR_QUEUED_FUNCTIONS) {
                EnqueueFunction(function);
            } else {
                LaunchThreadInternal(function);
            }
            return;
        }

        if (threadCount < opt::MIN_THREADS) {
            LaunchThreadInternal(function);
            return;
        }

        if (eventQueue.empty() || function->getBytecodeSize() < opt::SMALL_FUNCTION_BYTECODE_THRESHOLD) {
            EnqueueFunction(function);
            return;
        }

        if (threadCount < opt::MIN_THREADS_FOR_FILLED_EVENT_QUEUE &&
            eventQueue.size() > opt::MIN_EVENT_QUEUE_SIZE_BEFORE_THREAD_LAUNCH) {
            LaunchThreadInternal(function);
            return;
        }

        if (eventQueue.size() * opt::EVENT_QUEUE_THREAD_RATIO_THRESHOLD >= threadCount) {
            LaunchThreadInternal(function);
        } else {
            EnqueueFunction(function);
        }
    }

    static VThreadGroup* GetIdealVThreadGroup() {
        VThreadGroup* best = nullptr;
        for (const auto& [key, group] : vThreadGroups) {
            u64 threadCount = group->getThreadCount();

            if (threadCount == 0)
                return group.get();

            if (best == nullptr || threadCount < best->getThreadCount()) {
                best = group.get();
            }
        }

        return best;
    }

    // TODO: make a good scheduling system. currently it just does random shit. perhaps get the goat solar mist on the case
    void Schedule(Function* function) {
        if (function->getReturnType().getInternalType() != Type::VOID || !function->getArgumentTypes().empty()) {
            return;  // Only ()V functions can run on a thread
        }

        if (function->isNative()) {
            ScheduleOS(function);
            return;
        }

        u64 threadCount = ThreadCount();

        if (threadCount >= opt::MAX_CONCURRENT_THREADS) {
            VThreadGroup* group = GetIdealVThreadGroup();

            if (function->isPure() || function->getBytecodeSize() < opt::SMALL_FUNCTION_BYTECODE_THRESHOLD) {
                if (group->getThreadCount() < opt::MAX_VTHREADS) {
                    group->runFunction(function);
                } else {
                    EnqueueFunction(function);
                }
            } else {
                if (group->getThreadCount() < eventQueue.size()) {
                    group->runFunction(function);
                } else {
                    EnqueueFunction(function);
                }
            }
            return;
        }

        if (threadCount < opt::MIN_THREADS) {
            if (function->isPure() || function->getBytecodeSize() < opt::PURE_FUNCTION_BYTECODE_THRESHOLD) {
                VThreadGroup* group = GetIdealVThreadGroup();
                if (group == nullptr || group->getThreadCount() > opt::MIN_VTHREADS) {
                    group = LaunchVThreadGroup();
                }
                group->runFunction(function);
            } else {
                LaunchThreadInternal(function);
            }
            return;
        }

        if (function->isPure()) {
            if (vThreadGroups.size() > 8) {
                VThreadGroup* group = GetIdealVThreadGroup();
                if (group->getThreadCount() > 6) {
                    group = LaunchVThreadGroup();
                }
                group->runFunction(function);
            } else {
                VThreadGroup* group = LaunchVThreadGroup();
                group->runFunction(function);
            }
            return;
        } else if (function->getBytecodeSize() < opt::SMALL_FUNCTION_BYTECODE_THRESHOLD) {
            VThreadGroup* group = GetIdealVThreadGroup();
            if (group->getThreadCount() > opt::MIN_VTHREADS) {
                group = LaunchVThreadGroup();
            }
            group->runFunction(function);
            return;
        } else {
            LaunchThreadInternal(function);
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

    Executor& CurrentExecutor() {
        auto it = threads.find(std::this_thread::get_id());

        if (it != threads.end()) {
            return it->second->getExecutor();
        }

        auto it2 = vThreadGroups.find(std::this_thread::get_id());

        if (it2 != vThreadGroups.end()) {
            return it2->second->getExecutor();
        }

        std::cout << "error: no active thread. make sure to never use jesusvm from a native thread in native code\n";
        std::exit(1);
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
