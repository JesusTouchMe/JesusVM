#ifndef JESUSVM_THREAD_H
#define JESUSVM_THREAD_H

#include "JesusVM/executors/Threading.h"
#include "JesusVM/executors/VThread.h"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <optional>
#include <thread>
#include <variant>
#include <vector>

namespace JesusVM {
    class VThreadGroup;

    namespace Threading {
        static void LaunchThreadInternal(Function*);
        static VThreadGroup* LaunchVThreadGroup();
    }

    enum class ThreadState {
        NEW,
        RUNNABLE,
        TERMINATED,
        IDLE,
    };

    class Thread {
    friend void Threading::Init();
    friend void Threading::LaunchThreadInternal(Function*);
    public:
        Thread();

        std::thread::id getId() const;
        ThreadState getState() const;
        bool isDaemon() const;
        bool isInterrupted() const;

        Executor& getExecutor();

        MutationBuffer& getMutationBuffer();

        void setState(ThreadState state);
        void setFunction(Function* function);

        void yield();
        void sleep(Long ms);

        void start();
        void interrupt();

    private:
        std::thread::id mId;

        std::mutex mMutex;
        std::condition_variable mCondition;

        std::atomic<ThreadState> mState;
        bool mInterrupted;

        Executor mExecutor;
        Function* mFunction;

        MutationBuffer* mMutationBuffer;
    };

    class VThreadGroup {
    friend void Threading::Init();
    friend VThreadGroup* Threading::LaunchVThreadGroup();
    public:
        VThreadGroup();

        ThreadState getState() const;
        std::thread::id getId() const;
        bool isDaemon() const;
        bool isInterrupted() const;
        u64 getThreadCount() const;

        Executor& getExecutor();

        MutationBuffer& getMutationBuffer();

        void setState(ThreadState state);
        void runFunction(Function* function);

        void yield();
        void sleep(Long ms);

        void start();
        void interrupt();
        void interruptCurrent();

    private:
        std::thread::id mId;

        std::mutex mMutex;
        std::condition_variable mCondition;

        std::atomic<ThreadState> mState;
        bool mInterrupted;
        bool mCurrentInterrupted;

        std::vector<std::unique_ptr<VThread>> mThreads;
        VThread* mCurrent;

        MutationBuffer* mMutationBuffer;
    };
}

#endif // JESUSVM_THREAD_H