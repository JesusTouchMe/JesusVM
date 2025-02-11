// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_EXECUTORS_THREADING_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_EXECUTORS_THREADING_H

namespace JesusVM {
    class Thread;
}

namespace JesusVM::Threading {
    void Init(JesusVM& vm);

    Thread* LaunchThread();

    Thread* CurrentThread();

    u64 ThreadCount();
}

#endif //JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_EXECUTORS_THREADING_H
