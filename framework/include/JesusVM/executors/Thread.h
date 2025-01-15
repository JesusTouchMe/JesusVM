#ifndef JESUSVM_THREAD_H
#define JESUSVM_THREAD_H

#include "JesusVM/executors/VThread.h"

#include <mutex>
#include <vector>

namespace JesusVM {
	class JesusVM;

	class Thread {
	friend class JesusVM;
	public:
		Thread(JesusVM& mVM);

		u64 getThreadCount();

		VThread* getAvailableThread();

		void setMainThread();

		void run();
		void stop();

		void addVThread(std::unique_ptr<VThread> thread);

	private:
		std::vector<std::unique_ptr<VThread>> mVThreads;
		std::mutex mMutex;

		bool mRunning;

		JesusVM& mVM;
		bool mIsMainThread;

		VThread* mMainVThread;
	};
}

#endif // JESUSVM_THREAD_H