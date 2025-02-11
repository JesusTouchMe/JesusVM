#include "JesusVM/JesusVM.h"

#include <algorithm>

namespace JesusVM {
	JesusVM::JesusVM()
		: mMainThread(nullptr) {}

	void JesusVM::start(Function* main) {
		if (mMainThread == nullptr) {
			std::cout << "Major problem. The VM has no defined main thread. TODO: proper error\n";
			std::exit(1);
		}

		if (main->isNative()) {
			//TODO: thread system supporting native execution

			auto ptr = reinterpret_cast<NativeFunctionPtr<void>>(main->getEntry());
			ptr(nullptr, nullptr);
		} else {
			VThread* runner = mMainThread->getAvailableThread();
			if (runner == nullptr) {
                runner = mMainThread->addVThread();
			}

			mMainThread->mMainVThread = runner;
			runner->executeFunction(main);

			mMainThread->run();
		}
	}

	void JesusVM::stop() {
		for (auto& thread : mThreads) {
			thread->stop();
		}
	}

    VMContext JesusVM::getContext() {
        return reinterpret_cast<VMContext>(&mContext);
    }

	VThread* JesusVM::getAvailableThread() {
		if (mThreads.empty()) return nullptr;

		Thread* least = mThreads.front().get();
		for (auto& thread : mThreads) {
			VThread* vThread = thread->getAvailableThread();
			if (vThread != nullptr) {
				return vThread;
			}

			if (thread->getThreadCount() < least->getThreadCount()) {
				least = thread.get();
			}
		}

		return least->addVThread();
	}

	void JesusVM::addThread(std::unique_ptr<Thread> thread) {
		if (thread != nullptr) {
			mThreads.push_back(std::move(thread));
		}
	}
}