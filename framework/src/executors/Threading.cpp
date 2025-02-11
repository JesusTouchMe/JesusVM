// Copyright 2025 JesusTouchMe

#include "JesusVM/JesusVM.h"

#include "JesusVM/executors/Threading.h"

#include <condition_variable>
#include <mutex>

namespace JesusVM::Threading {
    JesusVM* globalVM = nullptr;

    std::unordered_map<std::thread::id, std::unique_ptr<Thread>> threads;

    std::mutex mutex;
    std::condition_variable terminateCondition;

    void Init(JesusVM& vm) {
        globalVM = &vm;
    }

    static void AttachThread(Thread* thread) {
        threads.emplace(thread->getId(), thread);
    }

    static void DetachThread(Thread* thread) {
        thread->setState(Thread::State::TERMINATED);

        threads.erase(thread->getId());

        terminateCondition.notify_all();
    }

    static void ThreadEntry(Thread* thread) {

    }

    void LaunchThread(Function* function) {
        auto thread = new Thread(*globalVM);

        if (function->isNative()) {
            thread->setMode(Thread::Mode::NATIVE_EXECUTOR);
        } else {
            thread->setMode(Thread::Mode::SINGLE_EXECUTOR);
        }

        thread->setState(Thread::State::RUNNABLE);

        std::lock_guard<std::mutex> lock(mutex);

        AttachThread(thread);

        thread->setFunction(function);

        std::thread nativeThread(ThreadEntry, thread);
        thread->mId = nativeThread.get_id();
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
            return threads.empty();
        });
    }
}
