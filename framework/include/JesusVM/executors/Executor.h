#ifndef JESUSVM_EXECUTOR_H
#define JESUSVM_EXECUTOR_H

#include "types.h"

#include "JesusVM/JesusNative.h"
#include "JesusVM/Stack.h"

#include "moduleweb/code_attribute.h"

namespace JesusVM {
	class VThread;

	class Executor {
    friend class Function;
	public:
		Executor();

        JValue getReturnValue() const;
        Stack& getStack();

        void run(); // run until return

		void executeInstruction(bool wide = false);

		void enterFunction(Function* function);

	private:
		Stack mStack;
		Stack::Frame* mFrame;

        u8* mCode;
		u32 mPC;

        i32 mReturnDepth;
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
        void shlInsn();
        void shrInsn();
		void landInsn();
		void lorInsn();
		void lxorInsn();
        void lshlInsn();
        void lshrInsn();

		void notInsn();
		void negInsn();
		void lnotInsn();
		void lnegInsn();

        void popInsn();

        void dupInsn();
        void dup2Insn();

        void swapInsn();
        void swap2Insn();

        void iloadInsn(bool wide);
        void istoreInsn(bool wide);

        void lloadInsn(bool wide);
        void lstoreInsn(bool wide);

        void hloadInsn(bool wide);
        void hstoreInsn(bool wide);

        void rloadInsn(bool wide);
        void rstoreInsn(bool wide);
        void rload_0Insn();

        void baloadInsn();
        void bastoreInsn();

        void caloadInsn();
        void castoreInsn();

        void saloadInsn();
        void sastoreInsn();

        void ialoadInsn();
        void iastoreInsn();

        void laloadInsn();
        void lastoreInsn();

        void haloadInsn();
        void hastoreInsn();

        void raloadInsn();
        void rastoreInsn();

        void arraylengthInsn();

        void newInsn(bool wide);
        void newarrayInsn();
        void rnewarrayInsn(bool wide);

        void isinstanceInsn(bool wide);

        void getfieldInsn(bool wide);
        void setfieldInsn(bool wide);

		void jmp_icmpeqInsn();
		void jmp_icmpneInsn();
		void jmp_icmpltInsn();
		void jmp_icmpgtInsn();
		void jmp_icmpleInsn();
		void jmp_icmpgeInsn();

        void jmp_hcmpeqInsn();
        void jmp_hcmpneInsn();

        void jmp_rcmpeqInsn();
        void jmp_rcmpneInsn();

        void jmp_hnullInsn();
        void jmp_hnonnullInsn();

        void jmp_rnullInsn();
        void jmp_rnonnullInsn();

		void jmpeqInsn();
		void jmpneInsn();
		void jmpltInsn();
		void jmpgtInsn();
		void jmpleInsn();
		void jmpgeInsn();
        void jmpInsn();

		void icmpInsn();
		void lcmpInsn();
        void hcmpInsn();
        void rcmpInsn();

        void bpushInsn();
		void spushInsn();
		void ipushInsn();
		void lpushInsn();

        void i2bInsn();
        void i2sInsn();
        void i2lInsn();
        void l2iInsn();

		void constInsn(i32 value);
		void lconstInsn(i64 value);

        void hconst_nullInsn();
        void rconst_nullInsn();

        void callInsn(bool wide);

		void returnInsn();
        void ireturnInsn();
        void lreturnInsn();
        void hreturnInsn();
        void rreturnInsn();

        void ldcInsn(bool wide);
	};
}

#endif // JESUSVM_EXECUTOR_H