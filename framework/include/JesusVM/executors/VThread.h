#ifndef JESUSVM_VTHREAD_H
#define JESUSVM_VTHREAD_H

#include "JesusVM/bytecode/Executor.h"

#include <chrono>

namespace JesusVM {
	class Thread;

	class VThread {
	friend class Executor;
	friend class Thread;
	public:
		VThread();

        Executor& getExecutor();

        bool isSleeping();

		void executeFunction(Function* function);

        void yield();
        void sleep(Long ms);

        void run(); // run until return

		void executeCycles(u32 cycles);
		void executeTime(u32 ms);

	private:
		Executor mExecutor;

        bool mIsYielded;

        bool mSleeping;
        std::chrono::steady_clock::time_point mWakeTime;
	};
}

#endif // JESUSVM_VTHREAD_H