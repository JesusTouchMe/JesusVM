// Copyright 2025 JesusTouchMe

#include "JesusVM/JesusVM.h"

#include "JesusVM/executors/Threading.h"

#include <condition_variable>
#include <mutex>

#undef VOID
#undef BOOL

namespace JesusVM::Threading {
    JesusVM* globalVM = nullptr;

    std::unordered_map<std::thread::id, std::unique_ptr<Thread>> threads;

    std::mutex mutex;
    std::condition_variable terminateCondition;

    u64 nonDaemonThreads = 0;

    static void AttachThread(Thread* thread) {
        threads.emplace(thread->getId(), thread);
    }

    static void DetachThread(Thread* thread) {
        thread->setState(Thread::State::TERMINATED);

        threads.erase(thread->getId());

        //TODO: if !thread.isDaemon
        nonDaemonThreads--;

        terminateCondition.notify_all();
    }

    static void ThreadEntry(Thread* thread) {
        thread->start();

        std::lock_guard<std::mutex> lock(mutex);
        DetachThread(thread);
    }

    void Init(JesusVM& vm) {
        globalVM = &vm;

        auto mainThread = new Thread(*globalVM, Thread::Mode::SINGLE_EXECUTOR); // TODO: maybe change the type to native executor in case main() is native
        mainThread->mId = std::this_thread::get_id();

        std::lock_guard<std::mutex> lock(mutex);

        AttachThread(mainThread); // mainThread is NOT a daemon thread, but we don't increment nonDaemonThreads since it would cause the program to last forever (main thread is never detached)
    }

    void LaunchThread(Function* function) {
        if (function->getReturnType().getInternalType() != Type::VOID || !function->getArgumentTypes().empty()) {
            return; // only ()V functions can run on a thread
                    // TODO: perhaps exceptions or special argument support
        }

        auto thread = new Thread(*globalVM, Thread::Mode::SINGLE_EXECUTOR);

        if (function->isNative()) {
            thread->setMode(Thread::Mode::NATIVE_EXECUTOR);
        }

        std::lock_guard<std::mutex> lock(mutex);

        nonDaemonThreads++; // TODO: if !thread.isDaemon or something
        AttachThread(thread);

        thread->setFunction(function);

        thread->setState(Thread::State::RUNNABLE);

        std::thread nativeThread(ThreadEntry, thread);
        thread->mId = nativeThread.get_id();

        nativeThread.detach();
    }

    Thread* CurrentThread() {
        auto it = threads.find(std::this_thread::get_id());

        if (it != threads.end()) {
            return it->second.get();
        }

        return nullptr;
    }

    u64 ThreadCount() {
        return threads.size();
    }

    void WaitForAllThreads() {
        std::unique_lock<std::mutex> lock(mutex);

        terminateCondition.wait(lock, [] {
            return nonDaemonThreads == 0;
        });
    }
}
