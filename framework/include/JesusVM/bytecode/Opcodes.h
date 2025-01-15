#ifndef JESUSVM_OPCODES_H
#define JESUSVM_OPCODES_H

#include "types.h"

namespace JesusVM {
	namespace Opcodes {
		enum EnumOpcodes : u8 {
			NOP,

			ADD,
			SUB,
			MUL,
			DIV,
			REM,
			LADD,
			LSUB,
			LMUL,
			LDIV,
			LREM,

			AND,
			OR,
			XOR,
			LAND,
			LOR,
			LXOR,

			NOT,
			NEG,
			LNOT,
			LNEG,

			JMP_ICMPEQ,
			JMP_ICMPNE,
			JMP_ICMPLT,
			JMP_ICMPGT,
			JMP_ICMPLE,
			JMP_ICMPGE,

			JMPEQ,
			JMPNE,
			JMPLT,
			JMPGT,
			JMPLE,
			JMPGE,

			ICMP,
			LCMP,
						
			BPUSH,
			SPUSH,
			IPUSH,
			LPUSH,

			CONST_M1,
			CONST_0,
			CONST_1,

			LCONST_0,
			LCONST_1,

            CALL,

			RETURN,
            IRETURN,
            LRETURN,

            WIDE,
		};
	}
}

#endif // JESUSVM_OPCODES_H
