#pragma once

#include "types.h"

#include "JesusVM/Stack.h"

namespace JesusVM {
	class Executor {
	public:
		Executor(Stack<0>& stack);

		void executeInstruction();

		void enterFunction(Function* function);

	private:
		Stack<0>& mStack;
		Stack<0>::Frame* mFrame;

		u8* mPC;

		u8 getByte();
		u16 getShort();
		u32 getInt();
		u64 getLong();

		void addInsn();
		void subInsn();
		void ipush8Insn();
		void printInsn();
	};
}