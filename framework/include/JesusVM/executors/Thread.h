#ifndef JESUSVM_THREAD_H
#define JESUSVM_THREAD_H

#include "JesusVM/Function.h"

#include "JesusVM/executors/Threading.h"
#include "JesusVM/executors/VThread.h"

#include <mutex>
#include <optional>
#include <variant>
#include <vector>

namespace JesusVM {
	class JesusVM;

    struct SingleExecutor {
        VThread executor; // to avoid useless rewriting of it
    };

    struct VThreadExecutor {
        std::vector<std::unique_ptr<VThread>> vThreads;
    };

    struct NativeExecutor {
        Function* function;
    };

    using ThreadMode = std::variant<std::monostate, SingleExecutor, VThreadExecutor, NativeExecutor>;

	class Thread {
	friend class JesusVM;
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
            NATIVE_EXECUTOR, // runs native functions. potentially in a queue
        };

		explicit Thread(JesusVM& mVM);

        Mode getMode() const;
        std::thread::id getId() const;

        void setState(State state);
        void setMode(Mode mode);

        void setFunction(Function* function);

		void run();
		void stop();

	private:
		std::mutex mMutex;

        Mode mMode;
        ThreadMode mThreadMode;

        std::atomic<State> mState;
		bool mRunning;

        std::thread::id mId; //

		JesusVM& mVM;
	};
}

#endif // JESUSVM_THREAD_H