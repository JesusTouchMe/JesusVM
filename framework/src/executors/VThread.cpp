#include "JesusVM/executors/VThread.h"

#include <chrono>

namespace JesusVM {
	VThread::VThread(JesusVM& vm)
		: mExecutor(vm, *this, mStack)
		, mIsActive(false) {}

	void VThread::executeFunction(Function* function) {
		mExecutor.enterFunction(function);

		mIsActive = true;
	}

	void VThread::executeCycles(u32 cycles) {
		for (u32 i = 0; i < cycles; i++) {
            if (!mIsActive) break;

			mExecutor.executeInstruction();
		}
	}

	void VThread::executeTime(u32 ms) {
		auto start = std::chrono::high_resolution_clock::now();
		auto end = start;
		std::chrono::duration<double, std::milli> elapsed;

		while (true) {
			end = std::chrono::high_resolution_clock::now();
			elapsed = end - start;

			if (elapsed.count() >= ms) {
				break;
			}

			mExecutor.executeInstruction();
		}
	}
}