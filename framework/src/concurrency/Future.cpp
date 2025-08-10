// Copyright 2025 JesusTouchMe

#include "JesusVM/concurrency/Future.h"

#include "JesusVM/concurrency/Scheduler.h"

namespace JesusVM {
    bool Future::isReady() const {
        std::lock_guard<std::mutex> lock(mMutex);
        return mReady;
    }

    void Future::wait() const {
        std::unique_lock<std::mutex> lock(mMutex);
        mCondition.wait(lock, [this]() { return mReady; });
    }

    std::optional<JValue> Future::getValue() const {
        std::unique_lock<std::mutex> lock(mMutex);
        return mValue;
    }

    void Future::addWaitingTask(Task* task) {
        std::unique_lock<std::mutex> lock(mMutex);
        mWaiting.push_back(task);
    }

    void Future::setValue(JValue value) {
        {
            std::lock_guard<std::mutex> lock(mMutex);
            if (mReady) return;

            mValue = value;
            mReady = true;
        }

        mCondition.notify_all();
        notifyWaitingTasks();
    }

    void Future::notifyWaitingTasks() {
        std::vector<Task*> toNotify;
        {
            std::lock_guard<std::mutex> lock(mMutex);
            toNotify.swap(mWaiting);
        }

        for (auto task : toNotify) {
            Scheduler::ResumeTask(task);
        }
    }
}
