// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_EXECUTORS_THREADING_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_EXECUTORS_THREADING_H

#include "types.h"

#include "JesusVM/executors/Daemon.h"
#include "JesusVM/bytecode/Executor.h"

namespace JesusVM {
    class Function;
    class Thread;

    struct IncrementMutation {
        Object* target;
        u64 epoch;

        void apply() const;
    };

    struct DecrementMutation {
        Object* target;
        u64 epoch;

        void apply() const;
    };

    struct MutationBuffer {
        std::vector<IncrementMutation> incrementMutations;
        std::vector<DecrementMutation> decrementMutations;
        u64 localEpoch = 0;

        MutationBuffer();

        void addReference(Object* object);
        void removeReference(Object* object);

        void advanceEpoch();

        void flush(u64 epoch);
    };
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

    Daemon* LaunchDaemon(std::string_view name, std::function<void(DaemonContext)> entry, bool crucial = false);

    // Only guarantees that the function will be run. Not how or when
    void Schedule(Function* function);

    Thread* GetMainThread();

    u64 MutatorThreadCount();
    u64 ThreadCount();

    void WaitForAllThreads();

    namespace CurrentThread {
        Handle GetHandle();

        Executor& GetExecutor();

        MutationBuffer& GetMutationBuffer();

        ThreadType GetType();

        bool IsNormalThread();
        bool IsVThreadGroup();
        bool IsSystemDaemon();

        bool IsDaemon(); // Not to be confused with IsSystemDaemon()
    }

    namespace Epoch {
        u64 GetGlobal();

        void SyncAllEpochs();

        void TryAdvanceGlobal();

        void FlushGlobalEpoch(u64 epoch);
    }
}

#endif //JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_EXECUTORS_THREADING_H
