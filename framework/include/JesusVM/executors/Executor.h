#ifndef JESUSVM_EXECUTOR_H
#define JESUSVM_EXECUTOR_H

#include "types.h"

#include "JesusVM/JesusNative.h"
#include "JesusVM/Stack.h"

#include "moduleweb/code_attribute.h"

namespace JesusVM {
    class JesusVM;
	class VThread;

	class Executor {
	public:
		explicit Executor(JesusVM& vm);

        JValue getReturnValue() const;

        void run(); // run until return

		void executeInstruction(bool wide = false);

		void enterFunction(Function* function);

	private:
        JesusVM& mVM;
		Stack mStack;
		Stack::Frame* mFrame;

		u8* mPC;

        bool mReturned;
        JValue mReturnValue;

		u8 getByte();
		u16 getShort();
		u32 getInt();
		u64 getLong();

		void addInsn();
		void subInsn();
		void mulInsn();
		void divInsn();
		void remInsn();
		void laddInsn();
		void lsubInsn();
		void lmulInsn();
		void ldivInsn();
		void lremInsn();

		void andInsn();
		void orInsn();
		void xorInsn();
		void landInsn();
		void lorInsn();
		void lxorInsn();

		void notInsn();
		void negInsn();
		void lnotInsn();
		void lnegInsn();

		void jmp_icmpeqInsn();
		void jmp_icmpneInsn();
		void jmp_icmpltInsn();
		void jmp_icmpgtInsn();
		void jmp_icmpleInsn();
		void jmp_icmpgeInsn();

		void jmpeqInsn();
		void jmpneInsn();
		void jmpltInsn();
		void jmpgtInsn();
		void jmpleInsn();
		void jmpgeInsn();
        void jmpInsn();

		void icmpInsn();
		void lcmpInsn();

		void bpushInsn();
		void spushInsn();
		void ipushInsn();
		void lpushInsn();

		void constInsn(i32 value);
		void lconstInsn(i64 value);

        void callInsn(bool wide);

		void returnInsn();
        void ireturnInsn();
        void lreturnInsn();
	};
}

#endif // JESUSVM_EXECUTOR_H