// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_EXECUTORS_THREADING_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_EXECUTORS_THREADING_H

#include "types.h"

namespace JesusVM {
    class Function;
    class Thread;
}

namespace JesusVM::Threading {
    void Init();

    // Will always launch a new thread
    void LaunchThread(Function* function);

    // Only guarantees that the function will be run. Not how or when
    void Schedule(Function* function);

    Handle CurrentThread();

    u64 ThreadCount();

    void WaitForAllThreads();
}

#endif //JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_EXECUTORS_THREADING_H
