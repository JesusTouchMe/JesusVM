// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_EXECUTORS_THREADING_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_EXECUTORS_THREADING_H

#include "types.h"

#include "JesusVM/executors/Daemon.h"
#include "JesusVM/executors/Executor.h"

namespace JesusVM {
    class Function;
    class Thread;
}

namespace JesusVM::Threading {
    enum class ThreadType {
        ERROR_TYPE,
        NORMAL,
        VTHREAD_GROUP,
        SYSTEM_DAEMON,
    };

    void Init();

    // Will always launch a new thread
    void LaunchThread(Function* function);

    Daemon* LaunchDaemon(std::string_view name, std::function<void(DaemonContext)> entry);

    // Only guarantees that the function will be run. Not how or when
    void Schedule(Function* function);

    u64 ThreadCount();

    void WaitForAllThreads();

    namespace CurrentThread {
        Handle GetHandle();

        Executor& GetExecutor();

        ThreadType GetType();

        bool IsNormalThread();
        bool IsVThreadGroup();
        bool IsSystemDaemon();

        bool IsDaemon(); // Not to be confused with IsSystemDaemon()
    }
}

#endif //JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_EXECUTORS_THREADING_H
