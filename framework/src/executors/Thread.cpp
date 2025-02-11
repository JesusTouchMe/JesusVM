#include "JesusVM/JesusVM.h"

#include "JesusVM/executors/Thread.h"

#include <algorithm>

namespace JesusVM {
	Thread::Thread(JesusVM& vm)
		: mRunning(false)
        , mState(State::NEW)
		, mVM(vm)
		, mIsMainThread(false) {}

	u64 Thread::getThreadCount() {
		return mVThreads.size();
	}

	VThread* Thread::getAvailableThread() {
		std::lock_guard<std::mutex> lock(mMutex);
		
		auto it = std::find_if(mVThreads.begin(), mVThreads.end(), [](auto& thread) { return !thread->mIsActive; });
		if (it != mVThreads.end()) return (*it).get();

		return nullptr;
	}

    VThread* Thread::addVThread() {
        auto thread = std::make_unique<VThread>(mVM);
        auto res = thread.get();

        mVThreads.push_back(std::move(thread));

        return res;
    }

	void Thread::run() {
		mRunning = true;
		
		std::vector<std::unique_ptr<VThread>>::size_type index = 0;

		while (mRunning) {
			VThread* currentThread = nullptr;
			
			{
				std::lock_guard<std::mutex> lock(mMutex);
				if (mVThreads.empty()) continue;

				currentThread = mVThreads[index].get();
				index = (index + 1) % mVThreads.size();
			}

			if (currentThread != nullptr && currentThread->mIsActive) {
				currentThread->executeCycles(5);
			}
		}
	}

	void Thread::stop() {
		mRunning = false;
	}
}