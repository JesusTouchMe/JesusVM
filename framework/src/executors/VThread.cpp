#include "JesusVM/executors/VThread.h"

#include <chrono>

namespace JesusVM {
	VThread::VThread(JesusVM& vm)
		: mExecutor(vm)
        , mIsYielded(false) {}

    Executor& VThread::getExecutor() {
        return mExecutor;
    }

    void VThread::run() {
        mExecutor.run();
    }

	void VThread::executeFunction(Function* function) {
		mExecutor.enterFunction(function);
	}

    void VThread::yield() {
        mIsYielded = true;
    }

	void VThread::executeCycles(u32 cycles) {
		for (u32 i = 0; i < cycles; i++) {
            if (mIsYielded) break;

			mExecutor.executeInstruction();
		}
	}

	void VThread::executeTime(u32 ms) {
		auto start = std::chrono::high_resolution_clock::now();
		auto end = start;
		std::chrono::duration<double, std::milli> elapsed{};

		while (true) {
			end = std::chrono::high_resolution_clock::now();
			elapsed = end - start;

			if (elapsed.count() >= ms || mIsYielded) {
				break;
			}

			mExecutor.executeInstruction();
		}
	}
}