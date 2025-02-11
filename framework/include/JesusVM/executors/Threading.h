// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_EXECUTORS_THREADING_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_EXECUTORS_THREADING_H

#include "types.h"

namespace JesusVM {
    class JesusVM;
    class Thread;
}

namespace JesusVM::Threading {
    void Init(JesusVM& vm);

    void LaunchThread(Function* function);

    Thread* CurrentThread();

    u64 ThreadCount();

    void WaitForAllThreads();
}

#endif //JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_EXECUTORS_THREADING_H
