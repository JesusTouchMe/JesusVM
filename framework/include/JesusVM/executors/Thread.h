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
	class JesusVM;

    struct SingleExecutor {
        Executor executor;
    };

    struct VThreadExecutor {
        std::vector<std::unique_ptr<VThread>> vThreads;
        VThread* current;
    };

    using ThreadMode = std::variant<std::monostate, SingleExecutor, VThreadExecutor>;

	class Thread {
	friend class JesusVM;
    friend void Threading::Init(JesusVM&);
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

		Thread(JesusVM& mVM, Mode mode);

        State getState() const;
        Mode getMode() const;
        std::thread::id getId() const;

        Executor& getExecutor();

        void setState(State state);
        void setMode(Mode mode);

        void setFunction(Function* function);

        void yield();

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

		JesusVM& mVM;

        bool mMainThread;
	};
}

#endif // JESUSVM_THREAD_H