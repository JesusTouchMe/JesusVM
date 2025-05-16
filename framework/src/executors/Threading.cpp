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

namespace JesusVM {
    void IncrementMutation::apply() const {
        target->addReferenceReal();
    }

    void DecrementMutation::apply() const {
        target->removeReferenceReal();
    }

    MutationBuffer::MutationBuffer() {
        localEpoch = Threading::Epoch::GetGlobal();
    }

    void MutationBuffer::addReference(Object* object) {
        incrementMutations.emplace_back(object, localEpoch);
    }

    void MutationBuffer::removeReference(Object* object) {
        decrementMutations.emplace_back(object, localEpoch);
    }

    void MutationBuffer::advanceEpoch() {
        localEpoch += 1;
        Threading::Epoch::TryAdvanceGlobal();
    }

    void MutationBuffer::flush(u64 epoch) {
        size_t writeIndex = 0;

        for (auto & mutation : incrementMutations) {
            if (mutation.epoch <= epoch) {
                mutation.apply();
            } else {
                incrementMutations[writeIndex++] = std::move(mutation);
            }
        }

        incrementMutations.resize(writeIndex);

        writeIndex = 0;

        for (auto & mutation : decrementMutations) {
            if (mutation.epoch <= epoch) {
                mutation.apply();
            } else {
                decrementMutations[writeIndex++] = std::move(mutation);
            }
        }

        decrementMutations.resize(writeIndex);
    }
}

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

    Thread* mainThread = nullptr;

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

    static void DaemonEntry(Daemon* thread, bool crucial) {
        thread->start();

        std::lock_guard<std::mutex> lock(mutex);
        if (crucial) nonDaemonThreads--;
        DetachDaemon(thread);
    }

    void Init() {
        auto mainThreadPtr = std::make_unique<Thread>();
        mainThreadPtr->mId = std::this_thread::get_id();

        mainThread = mainThreadPtr.get();

        {
            std::lock_guard<std::mutex> lock(mutex);

            AttachThread(std::move(mainThreadPtr));
            nonDaemonThreads--; // mainThread is NOT a daemon thread, but we don't increment nonDaemonThreads since it would cause the program to last forever (main thread is never detached)
        }

        // Launch system daemons. Most are paused at entry, but still need to be launched here
        //GC::Daemon::Launch();
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

    Daemon* LaunchDaemon(std::string_view name, std::function<void(DaemonContext)> entry, bool crucial) {
        auto thread = std::make_unique<Daemon>(name, std::move(entry));
        auto* tmpThread = thread.get();
        std::thread nativeThread(DaemonEntry, tmpThread, crucial);

        thread->mId = nativeThread.get_id();

        {
            std::lock_guard<std::mutex> lock(mutex);
            AttachDaemon(std::move(thread));

            if (crucial) nonDaemonThreads++;
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

    Thread* GetMainThread() {
        return mainThread;
    }

    u64 MutatorThreadCount() {
        return threads.size() + vThreadGroups.size();
    }

    u64 ThreadCount() {
        return threads.size() + vThreadGroups.size() + daemons.size();
    }

    void WaitForAllThreads() {
        std::unique_lock<std::mutex> lock(mutex);

        terminateCondition.wait(lock, [] {
            return nonDaemonThreads == 0;
        });
    }

    static inline void ForEachMutator(const auto& consumer) {
        for (auto& thread : threads) {
            consumer(thread.second->getMutationBuffer());
        }

        for (auto& thread : vThreadGroups) {
            consumer(thread.second->getMutationBuffer());
        }
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

        MutationBuffer& GetMutationBuffer() {
            static thread_local MutationBuffer buffer;
            //TODO: if GetType() == ERROR_TYPE throw error
            return buffer;
        }

        ThreadType GetType() {
            if (threads.contains(id())) return ThreadType::NORMAL;
            if (vThreadGroups.contains(id())) return ThreadType::VTHREAD_GROUP;
            if (daemons.contains(id())) return ThreadType::SYSTEM_DAEMON;
            return ThreadType::ERROR_TYPE;
        }

        bool IsNormalThread() {
            return threads.contains(id());
        }

        bool IsVThreadGroup() {
            return vThreadGroups.contains(id());
        }

        bool IsSystemDaemon() {
            return daemons.contains(id());
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

    namespace Epoch {
        std::atomic<u64> globalEpoch = 0;
        constexpr double advanceThreshold = 0.9;

        u64 GetGlobal() {
            return globalEpoch;
        }

        void SyncAllEpochs() {
            u64 maxEpoch = globalEpoch.load(std::memory_order_acquire);

            ForEachMutator([&maxEpoch](MutationBuffer& buffer) {
                if (buffer.localEpoch > maxEpoch) {
                    maxEpoch = buffer.localEpoch;
                }
            });

            for (u64 epoch = globalEpoch; epoch <= maxEpoch; epoch++) {
                FlushGlobalEpoch(epoch);
            }

            //TODO: fix the daemon thread having weird behavior and not freeing stuff or running
            //GC::Daemon::WaitUntilReady();
            //GC::Daemon::Finalize(); // signal that it should finish after this final collection
            //GC::Daemon::BeginCollection();

            GC::ProcessCycles();
        }

        void TryAdvanceGlobal() {
            u64 currentGlobal = globalEpoch.load(std::memory_order_acquire);
            u64 readyThreads = 0;
            u64 totalThreads = 0;

            ForEachMutator([currentGlobal, &totalThreads, &readyThreads](MutationBuffer& buffer) {
                totalThreads += 1;
                if (buffer.localEpoch > currentGlobal) {
                    readyThreads += 1;
                }
            });

            if (totalThreads == 0) return;

            double readiness = static_cast<double>(readyThreads) / totalThreads;
            if (readiness >= advanceThreshold) {
                if (globalEpoch.compare_exchange_strong(currentGlobal, currentGlobal + 1, std::memory_order_acq_rel)) {
                    static std::mutex mutex;
                    std::lock_guard<std::mutex> lock(mutex);

                    FlushGlobalEpoch(currentGlobal);

                    GC::ProcessCycles();
                }
            }
        }

        void FlushGlobalEpoch(u64 epoch) {
            ForEachMutator([epoch](MutationBuffer& buffer) {
                buffer.flush(epoch);
            });
        }
    }
}
