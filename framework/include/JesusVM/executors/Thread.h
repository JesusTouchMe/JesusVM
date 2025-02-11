#ifndef JESUSVM_THREAD_H
#define JESUSVM_THREAD_H

#include "JesusVM/executors/Threading.h"
#include "JesusVM/executors/VThread.h"

#include <mutex>
#include <vector>

namespace JesusVM {
	class JesusVM;

    enum class ThreadState {
        NEW,
        RUNNABLE,
        TERMINATED,
    };

	class Thread {
	friend class JesusVM;
    friend Thread* Threading::LaunchThread();
	public:
        struct Env {
            Thread* thread;
        };

		explicit Thread(JesusVM& mVM);

		u64 getThreadCount();

		VThread* getAvailableThread();
        VThread* addVThread();

		void run();
		void stop();

	private:
		std::vector<std::unique_ptr<VThread>> mVThreads;
		std::mutex mMutex;

        std::atomic<ThreadState> mState;
		bool mRunning;
        std::thread::id mId;

        std::unique_ptr<Env> mEnv;

		JesusVM& mVM;
		bool mIsMainThread;

		VThread* mMainVThread;
	};
}

#endif // JESUSVM_THREAD_H