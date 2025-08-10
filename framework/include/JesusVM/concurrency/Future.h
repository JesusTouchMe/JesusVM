// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_CONCURRENCY_FUTURE_H
#define JESUSVM_CONCURRENCY_FUTURE_H 1


#include "JesusVM/JesusNative.h"

#include "JesusVM/concurrency/Task.h"

#include <condition_variable>
#include <optional>
#include <vector>

namespace JesusVM {
    class Promise;

    class Future {
    friend class Promise;
    public:
        bool isReady() const;
        void wait() const; // blocking

        std::optional<JValue> getValue() const;

        void addWaitingTask(Task* task);

    private:
        bool mReady = false;
        std::optional<JValue> mValue;

        std::vector<Task*> mWaiting;

        mutable std::mutex mMutex;
        mutable std::condition_variable mCondition;

        void setValue(JValue value);
        void notifyWaitingTasks();
    };
}

#endif // JESUSVM_CONCURRENCY_FUTURE_H
