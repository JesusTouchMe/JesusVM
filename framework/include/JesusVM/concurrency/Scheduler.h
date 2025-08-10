// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_CONCURRENCY_SCHEDULER_H
#define JESUSVM_CONCURRENCY_SCHEDULER_H 1

#include "JesusVM/Function.h"

#include "JesusVM/concurrency/Task.h"

namespace JesusVM::Scheduler {
    void Schedule(Function* function);

    void ScheduleTask(std::unique_ptr<Task> task);

    void PauseCurrentTaskOnFuture(Future* future);

    void ResumeTask(Task* task);

    void Run();
}

#endif // JESUSVM_CONCURRENCY_SCHEDULER_H
