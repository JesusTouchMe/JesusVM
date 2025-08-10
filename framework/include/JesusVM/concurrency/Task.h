// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_CONCURRENCY_TASK_H
#define JESUSVM_CONCURRENCY_TASK_H 1

#include "JesusVM/Function.h"

#include "JesusVM/bytecode/Executor.h"

namespace JesusVM {
    class Future;

    class Task {
    public:
        Task(Function* function, Future* future);

        void run();

        bool isComplete() const;

        Future* getFuture() const;

    private:
        Executor mExecutor;
        Future* mFuture;
    };
}

#endif // JESUSVM_CONCURRENCY_TASK_H
