#include "JesusVM/executors/VThread.h"

#include <chrono>

namespace JesusVM {
	VThread::VThread()
		: mExecutor()
        , mIsYielded(false)
        , mSleeping(false) {}

    Executor& VThread::getExecutor() {
        return mExecutor;
    }

    bool VThread::isSleeping() {
        if (mSleeping && std::chrono::steady_clock::now() >= mWakeTime) {
            mSleeping = false;
        }

        return mSleeping;
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

    void VThread::sleep(Long ms) {
        mSleeping = true;
        mWakeTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);
    }

	void VThread::executeCycles(u32 cycles) {
        mIsYielded = false;

		for (u32 i = 0; i < cycles; i++) {
            if (mIsYielded || isSleeping()) break;

			mExecutor.executeInstruction();
		}
	}

	void VThread::executeTime(u32 ms) {
        mIsYielded = false;

		auto start = std::chrono::high_resolution_clock::now();
		auto end = start;
		std::chrono::duration<double, std::milli> elapsed{};

		while (true) {
			end = std::chrono::high_resolution_clock::now();
			elapsed = end - start;

			if (elapsed.count() >= ms || mIsYielded || isSleeping()) {
				break;
			}

			mExecutor.executeInstruction();
		}
	}
}