#ifndef JESUSVM_VTHREAD_H
#define JESUSVM_VTHREAD_H

#include "JesusVM/executors/Executor.h"

namespace JesusVM {
	class Thread;

	class VThread {
	friend class Executor;
	friend class Thread;
	public:
		explicit VThread(JesusVM& vm);

        Executor& getExecutor();

		void executeFunction(Function* function);

        void yield();

        void run(); // run until return

		void executeCycles(u32 cycles);
		void executeTime(u32 ms);

	private:
		Executor mExecutor;

        bool mIsYielded;
	};
}

#endif // JESUSVM_VTHREAD_H