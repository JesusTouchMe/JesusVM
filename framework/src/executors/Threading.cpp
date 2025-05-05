// Copyright 2025 JesusTouchMe

#include "JesusVM/JesusVM.h"

#include "JesusVM/executors/Threading.h"
#include "JesusVM/executors/Thread.h"

#include "JesusVM/heap/gc/GC-Daemon.h"

#include <condition_variable>
#include <mutex>
#include <queue>

#undef VOID
#undef BOOL

namespace JesusVM::Threading {
    std::unordered_map<std::thread::id, std::unique_ptr<Thread>> threads;
    std::unordered_map<std::thread::id, std::unique_ptr<VThreadGroup>> vThreadGroups;
    std::unordered_map<std::thread::id, std::unique_ptr<Daemon>> daemons;

    std::queue<Function*> eventQueue;
    std::mutex eventQueueMutex;
    std::condition_variable eventQueueCondition;

    std::mutex mutex;
    std::condition_variable terminateCondition;

    u64 nonDaemonThreads = 0;

    static void AttachThread(std::unique_ptr<Thread> thread) {
        if (!thread->isDaemon()) {
            nonDaemonThreads++;
        }

        threads[thread->getId()] = std::move(thread);
    }

    static void AttachThread(std::unique_ptr<VThreadGroup> thread) {
        if (!thread->isDaemon()) {
            nonDaemonThreads++;
        }

        vThreadGroups[thread->getId()] = std::move(thread);
    }

    static void AttachDaemon(std::unique_ptr<Daemon> thread) {
        daemons[thread->getId()] = std::move(thread);
    }

    static void DetachThread(Thread* thread) {
        thread->setState(ThreadState::TERMINATED);

        threads.erase(thread->getId());

        if (!thread->isDaemon()) {
            nonDaemonThreads--;
        }

        terminateCondition.notify_all();
    }

    static void DetachThread(VThreadGroup* thread) {
        thread->setState(ThreadState::TERMINATED);

        vThreadGroups.erase(thread->getId());

        if (!thread->isDaemon()) {
            nonDaemonThreads--;
        }

        terminateCondition.notify_all();
    }

    static void DetachDaemon(Daemon* daemon) {
        daemons.erase(daemon->getId());
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

    static void DaemonEntry(Daemon* thread) {
        thread->start();
        DetachDaemon(thread);
    }

    void Init() {
        auto mainThread = std::make_unique<Thread>();
        mainThread->mId = std::this_thread::get_id();

        {
            std::lock_guard<std::mutex> lock(mutex);

            AttachThread(std::move(mainThread));
            nonDaemonThreads--; // mainThread is NOT a daemon thread, but we don't increment nonDaemonThreads since it would cause the program to last forever (main thread is never detached)
        }

        // Launch system daemons. Most are paused at entry, but still need to be launched here
        GC::Daemon::Launch();
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

    Daemon* LaunchDaemon(std::string_view name, std::function<void(DaemonContext)> entry) {
        auto thread = std::make_unique<Daemon>(name, std::move(entry));
        auto* tmpThread = thread.get();
        std::thread nativeThread(DaemonEntry, tmpThread);

        thread->mId = nativeThread.get_id();

        {
            std::lock_guard<std::mutex> lock(mutex);
            AttachDaemon(std::move(thread));
        }

        nativeThread.detach();

        tmpThread->wake();

        return tmpThread;
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

    u64 ThreadCount() {
        return threads.size() + vThreadGroups.size();
    }

    void WaitForAllThreads() {
        std::unique_lock<std::mutex> lock(mutex);

        terminateCondition.wait(lock, [] {
            return nonDaemonThreads == 0;
        });
    }

    namespace CurrentThread {
        static inline auto id() {
            return std::this_thread::get_id();
        }

        Handle GetHandle() {
            auto it = threads.find(id());
            if (it != threads.end()) {
                return it->second.get();
            }

            auto it2 = vThreadGroups.find(id());
            if (it2 != vThreadGroups.end()) {
                return it2->second.get();
            }

            auto it3 = daemons.find(id());
            if (it3 != daemons.end()) {
                return it3->second.get();
            }

            return nullptr;
        }

        Executor& GetExecutor() {
            auto it = threads.find(id());

            if (it != threads.end()) {
                return it->second->getExecutor();
            }

            auto it2 = vThreadGroups.find(id());

            if (it2 != vThreadGroups.end()) {
                return it2->second->getExecutor();
            }

            std::cout << "error: no active thread. make sure to never use jesusvm from a native thread in native code\n";
            std::exit(1);
        }

        ThreadType GetType() {
            if (threads.contains(id())) return ThreadType::NORMAL;
            if (vThreadGroups.contains(id())) return ThreadType::VTHREAD_GROUP;
            if (daemons.contains(id())) return ThreadType::SYSTEM_DAEMON;
            return ThreadType::ERROR_TYPE;
        }

        bool IsNormalThread() {
            return GetType() == ThreadType::NORMAL;
        }

        bool IsVThreadGroup() {
            return GetType() == ThreadType::VTHREAD_GROUP;
        }

        bool IsSystemDaemon() {
            return GetType() == ThreadType::SYSTEM_DAEMON;
        }

        bool IsDaemon() {
            auto it = threads.find(id());
            if (it != threads.end()) {
                return it->second->isDaemon();
            }

            auto it2 = vThreadGroups.find(id());
            if (it2 != vThreadGroups.end()) {
                return it2->second->isDaemon();
            }

            return daemons.contains(id());
        }
    }
}
