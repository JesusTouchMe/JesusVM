#ifndef JESUSVM_VTHREAD_H
#define JESUSVM_VTHREAD_H

#include "JesusVM/Stack.h"

#include "JesusVM/executors/Executor.h"

namespace JesusVM {
	class Thread;

	class VThread {
	friend class Executor;
	friend class Thread;
	public:
		explicit VThread(JesusVM& vm);

		void executeFunction(Function* function);

        void run(); // run until return

		void executeCycles(u32 cycles);
		void executeTime(u32 ms);

	private:
		Stack mStack;
		Executor mExecutor;

		bool mIsActive;
	};
}

#endif // JESUSVM_VTHREAD_H