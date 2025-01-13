#pragma once

#include "JesusVM/Stack.h"

#include "JesusVM/executors/Executor.h"

namespace JesusVM {
	class Thread;

	class VThread {
	friend class Executor;
	friend class Thread;
	public:
		VThread();

		void executeFunction(Function* function);

		void executeCycles(u32 cycles);
		void executeTime(u32 ms);

	private:
		Stack<0> mStack;
		Executor mExecutor;

		bool mIsActive;
	};
}
