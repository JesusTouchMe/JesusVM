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

        mMainThread->setFunction(main);
	}

	void JesusVM::stop() {
        Threading::WaitForAllThreads();
	}

    VMContext JesusVM::getContext() {
        return reinterpret_cast<VMContext>(&mContext);
    }
}