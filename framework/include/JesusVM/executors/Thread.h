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
    namespace Threading {
        static inline void LaunchThreadInternal(Function*);
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
    };

    class VThreadGroup {
    friend void Threading::Init();
    friend void Threading::LaunchThread(Function*);
    public:
        VThreadGroup();

        ThreadState getState() const;
        std::thread::id getId() const;
        bool isDaemon() const;
        bool isInterrupted() const;
        u64 getThreadCount() const;

        Executor& getExecutor();

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
    };

    /*
    struct SingleExecutor {
        Executor executor;
    };

    struct VThreadExecutor {
        std::vector<std::unique_ptr<VThread>> vThreads;
        VThread* current;
    };

    using ThreadMode = std::variant<SingleExecutor, VThreadExecutor>;

	class Thread {
    friend void Threading::Init();
    friend void Threading::LaunchThread(Function*);
	public:
        enum class State {
            NEW,
            RUNNABLE,
            TERMINATED,
            IDLE,
        };

        enum class Mode {
            SINGLE_EXECUTOR, // runs a single thing of bytecode
            VTHREAD_EXECUTOR, // runs a list of vthreads and stays idle if there are none
        };

		explicit Thread(Mode mode);

        State getState() const;
        Mode getMode() const;
        std::thread::id getId() const;

        Executor& getExecutor();

        void setState(State state);
        void setMode(Mode mode);

        void setFunction(Function* function);

        // Methods that will be called by a threading module eventually
        void yield();
        void sleep(Long ms);

		void start();
		void stop();

	private:
		std::mutex mMutex;
        std::condition_variable mCondition;

        Mode mMode;
        ThreadMode mThreadMode;

        std::atomic<State> mState;
		bool mInterrupted;

        std::thread::id mId;

        bool mMainThread;
	};
    */
}

#endif // JESUSVM_THREAD_H