 #include "JesusVM/JesusVM.h"

#include "JesusVM/bytecode/Opcodes.h"

#include "JesusVM/constpool/ConstantClass.h"
#include "JesusVM/constpool/ConstantField.h"
#include "JesusVM/constpool/ConstantFunc.h"
#include "JesusVM/constpool/ConstantGlobalVar.h"
#include "JesusVM/constpool/ConstPool.h"

#include "JesusVM/executors/Executor.h"
#include "JesusVM/executors/VThread.h"

#include <ranges>

#undef VOID
#undef BOOL

#ifdef JESUSVM_USE_JUMP_THREADING

#define Dispatch(o) goto *jumptable[o];
#define Case(o) L_ ## o
#define Default L_default
#define Break return

#define Insn(opcode) [Opcodes::opcode] = &&L_##opcode

#else

#define Dispatch(o) switch (o)
#define Case(o) case Opcodes::o
#define Default default
#define Break break

#endif

namespace JesusVM {
    constexpr u8 ReadU8(const u8* data, size_t index) {
        return data[index];
    }

    constexpr u16 ReadU16(const u8* data, size_t index) {
        return (static_cast<u16>(data[index]) << 8) |
               (static_cast<u16>(data[index + 1]) << 0);
    }

    constexpr u32 ReadU32(const u8* data, size_t index) {
        return (static_cast<u32>(data[index]) << 24) |
               (static_cast<u32>(data[index + 1]) << 16) |
               (static_cast<u32>(data[index + 2]) << 8) |
               (static_cast<u32>(data[index + 3]) << 0);
    }

    constexpr u64 ReadU64(const u8* data, size_t index) {
        return (static_cast<u64>(data[index]) << 56) |
               (static_cast<u64>(data[index + 1]) << 48) |
               (static_cast<u64>(data[index + 2]) << 40) |
               (static_cast<u64>(data[index + 3]) << 32) |
               (static_cast<u64>(data[index + 4]) << 24) |
               (static_cast<u64>(data[index + 5]) << 16) |
               (static_cast<u64>(data[index + 6]) << 8) |
               (static_cast<u64>(data[index + 7]) << 0);
    }

	Executor::Executor()
		: mFrame(mStack.getTopFrame())
        , mReturnDepth(0)
        , mReturnValue(0)
		, mPC(0)
        , mCode(nullptr) {}

    JValue Executor::getReturnValue() const {
        return mReturnValue;
    }

    Stack& Executor::getStack() {
        return mStack;
    }

    void Executor::run() {
        i32 savedDepth = mReturnDepth - 1;

        while (savedDepth < mReturnDepth) {
            executeInstruction();
        }
    }

	void Executor::executeInstruction(bool wide) {
        if (mPC >= mFrame->getCurrentFunction()->getBytecodeSize()) {
            std::cout << "bad execution!!!\n";
            std::exit(1);
        }

        //<editor-fold desc="jumptable">
#ifdef JESUSVM_USE_JUMP_THREADING
            static void* jumptable[] = {
                Insn(NOP),
                Insn(ADD),
                Insn(SUB),
                Insn(MUL),
                Insn(DIV),
                Insn(REM),
                Insn(LADD),
                Insn(LSUB),
                Insn(LMUL),
                Insn(LDIV),
                Insn(LREM),
                Insn(AND),
                Insn(OR),
                Insn(XOR),
                Insn(SHL),
                Insn(SHR),
                Insn(LAND),
                Insn(LOR),
                Insn(LXOR),
                Insn(LSHL),
                Insn(LSHR),
                Insn(NOT),
                Insn(NEG),
                Insn(LNOT),
                Insn(LNEG),
                Insn(POP),
                Insn(POP2),
                Insn(DUP),
                Insn(DUP2),
                Insn(SWAP),
                Insn(SWAP2),
                Insn(ILOAD),
                Insn(ISTORE),
                Insn(LLOAD),
                Insn(LSTORE),
                Insn(HLOAD),
                Insn(HSTORE),
                Insn(RLOAD),
                Insn(RSTORE),
                Insn(RLOAD_0),
                Insn(BALOAD),
                Insn(BASTORE),
                Insn(CALOAD),
                Insn(CASTORE),
                Insn(SALOAD),
                Insn(SASTORE),
                Insn(IALOAD),
                Insn(IASTORE),
                Insn(LALOAD),
                Insn(LASTORE),
                Insn(HALOAD),
                Insn(HASTORE),
                Insn(RALOAD),
                Insn(RASTORE),
                Insn(ARRAYLENGTH),
                Insn(NEW),
                Insn(NEWARRAY),
                Insn(RNEWARRAY),
                Insn(ISINSTANCE),
                Insn(GETFIELD),
                Insn(SETFIELD),
                Insn(GETGLOBAL),
                Insn(SETGLOBAL),
                Insn(JMP_ICMPEQ),
                Insn(JMP_ICMPNE),
                Insn(JMP_ICMPLT),
                Insn(JMP_ICMPGT),
                Insn(JMP_ICMPLE),
                Insn(JMP_ICMPGE),
                Insn(JMP_HCMPEQ),
                Insn(JMP_HCMPNE),
                Insn(JMP_RCMPEQ),
                Insn(JMP_RCMPNE),
                Insn(JMP_HNULL),
                Insn(JMP_HNONNULL),
                Insn(JMP_RNULL),
                Insn(JMP_RNONNULL),
                Insn(JMPEQ),
                Insn(JMPNE),
                Insn(JMPLT),
                Insn(JMPGT),
                Insn(JMPLE),
                Insn(JMPGE),
                Insn(JMP),
                Insn(ICMP),
                Insn(LCMP),
                Insn(HCMP),
                Insn(RCMP),
                Insn(BPUSH),
                Insn(SPUSH),
                Insn(IPUSH),
                Insn(LPUSH),
                Insn(I2B),
                Insn(I2S),
                Insn(I2L),
                Insn(L2I),
                Insn(CONST_M1),
                Insn(CONST_0),
                Insn(CONST_1),
                Insn(LCONST_0),
                Insn(LCONST_1),
                Insn(HCONST_NULL),
                Insn(RCONST_NULL),
                Insn(CALL),
                Insn(RETURN),
                Insn(IRETURN),
                Insn(LRETURN),
                Insn(HRETURN),
                Insn(RRETURN),
                Insn(LDC),
                Insn(WIDE),
                Insn(BREAKPOINT),
                Insn(RESERVE1),
                Insn(RESERVE2),

        };
#endif
        //</editor-fold>

		u8 instruction = mCode[mPC++];

        // TODO: Static analysis to find bad opcodes and shit
		Dispatch(instruction) {
            Case(NOP):
                Break;

            Case(ADD):
				addInsn();
				Break;

            Case(SUB):
				subInsn();
				Break;

            Case(MUL):
                mulInsn();
                Break;

            Case(DIV):
                divInsn();
                Break;

            Case(REM):
                remInsn();
                Break;

            Case(LADD):
                laddInsn();
                Break;

            Case(LSUB):
                lsubInsn();
                Break;

            Case(LMUL):
                lmulInsn();
                Break;

            Case(LDIV):
                ldivInsn();
                Break;

            Case(LREM):
                lremInsn();
                Break;

            Case(AND):
                andInsn();
                Break;

            Case(OR):
                orInsn();
                Break;

            Case(XOR):
                xorInsn();
                Break;

            Case(SHL):
                shlInsn();
                Break;

            Case(SHR):
                shrInsn();
                Break;

            Case(LAND):
                landInsn();
                Break;

            Case(LOR):
                lorInsn();
                Break;

            Case(LXOR):
                lxorInsn();
                Break;

            Case(LSHL):
                lshlInsn();
                Break;

            Case(LSHR):
                lshrInsn();
                Break;

            Case(NOT):
                notInsn();
                Break;

            Case(NEG):
                negInsn();
                Break;

            Case(LNOT):
                lnotInsn();
                Break;

            Case(LNEG):
                lnegInsn();
                Break;

            Case(POP):
            Case(POP2):
                popInsn();
                Break;

            Case(DUP):
                dupInsn();
                Break;

            Case(DUP2):
                dup2Insn();
                Break;

            Case(SWAP):
                swapInsn();
                Break;

            Case(SWAP2):
                swap2Insn();
                Break;

            Case(ILOAD):
                iloadInsn(wide);
                Break;

            Case(ISTORE):
                istoreInsn(wide);
                Break;

            Case(LLOAD):
                lloadInsn(wide);
                Break;

            Case(LSTORE):
                lstoreInsn(wide);
                Break;

            Case(HLOAD):
                hloadInsn(wide);
                Break;

            Case(HSTORE):
                hstoreInsn(wide);
                Break;

            Case(RLOAD):
                rloadInsn(wide);
                Break;

            Case(RSTORE):
                rstoreInsn(wide);
                Break;

            Case(RLOAD_0):
                rload_0Insn();
                Break;

            Case(BALOAD):
                baloadInsn();
                Break;

            Case(BASTORE):
                bastoreInsn();
                Break;

            Case(CALOAD):
                caloadInsn();
                Break;

            Case(CASTORE):
                castoreInsn();
                Break;

            Case(SALOAD):
                saloadInsn();
                Break;

            Case(SASTORE):
                sastoreInsn();
                Break;

            Case(IALOAD):
                ialoadInsn();
                Break;

            Case(IASTORE):
                iastoreInsn();
                Break;

            Case(LALOAD):
                laloadInsn();
                Break;

            Case(LASTORE):
                lastoreInsn();
                Break;

            Case(HALOAD):
                haloadInsn();
                Break;

            Case(HASTORE):
                hastoreInsn();
                Break;

            Case(RALOAD):
                raloadInsn();
                Break;

            Case(RASTORE):
                rastoreInsn();
                Break;

            Case(ARRAYLENGTH):
                arraylengthInsn();
                Break;

            Case(NEW):
                newInsn(wide);
                Break;

            Case(NEWARRAY):
                newarrayInsn();
                Break;

            Case(RNEWARRAY):
                rnewarrayInsn(wide);
                Break;

            Case(ISINSTANCE):
                isinstanceInsn(wide);
                Break;

            Case(GETFIELD):
                getfieldInsn(wide);
                Break;

            Case(SETFIELD):
                setfieldInsn(wide);
                Break;

            Case(GETGLOBAL):
                getglobalInsn(wide);
                Break;

            Case(SETGLOBAL):
                setglobalInsn(wide);
                Break;

            Case(JMP_ICMPEQ):
                jmp_icmpeqInsn();
                Break;

            Case(JMP_ICMPNE):
                jmp_icmpneInsn();
                Break;

            Case(JMP_ICMPLT):
                jmp_icmpltInsn();
                Break;

            Case(JMP_ICMPGT):
                jmp_icmpgtInsn();
                Break;

            Case(JMP_ICMPLE):
                jmp_icmpleInsn();
                Break;

            Case(JMP_ICMPGE):
                jmp_icmpgeInsn();
                Break;

            Case(JMP_HCMPEQ):
                jmp_hcmpeqInsn();
                Break;

            Case(JMP_HCMPNE):
                jmp_hcmpneInsn();
                Break;

            Case(JMP_RCMPEQ):
                jmp_rcmpeqInsn();
                Break;

            Case(JMP_RCMPNE):
                jmp_rcmpneInsn();
                Break;

            Case(JMP_HNULL):
                jmp_hnullInsn();
                Break;

            Case(JMP_HNONNULL):
                jmp_hnonnullInsn();
                Break;

            Case(JMP_RNULL):
                jmp_rnullInsn();
                Break;

            Case(JMP_RNONNULL):
                jmp_rnonnullInsn();
                Break;

            Case(JMPEQ):
                jmpeqInsn();
                Break;

            Case(JMPNE):
                jmpneInsn();
                Break;

            Case(JMPLT):
                jmpltInsn();
                Break;

            Case(JMPGT):
                jmpgtInsn();
                Break;

            Case(JMPLE):
                jmpleInsn();
                Break;

            Case(JMPGE):
                jmpgeInsn();
                Break;

            Case(JMP):
                jmpInsn();
                Break;

            Case(ICMP):
                icmpInsn();
                Break;

            Case(LCMP):
                lcmpInsn();
                Break;

            Case(HCMP):
                hcmpInsn();
                Break;

            Case(RCMP):
                rcmpInsn();
                Break;

            Case(BPUSH):
                bpushInsn();
                Break;

            Case(SPUSH):
                spushInsn();
                Break;

            Case(IPUSH):
                ipushInsn();
                Break;

            Case(LPUSH):
                lpushInsn();
                Break;

            Case(I2B):
                i2bInsn();
                Break;

            Case(I2S):
                i2sInsn();
                Break;

            Case(I2L):
                i2lInsn();
                Break;

            Case(L2I):
                l2iInsn();
                Break;

            Case(CONST_M1):
                constInsn(-1);
                Break;

            Case(CONST_0):
                constInsn(0);
                Break;

            Case(CONST_1):
                constInsn(1);
                Break;

            Case(LCONST_0):
                lconstInsn(0);
                Break;

            Case(LCONST_1):
                lconstInsn(1);
                Break;

            Case(HCONST_NULL):
                hconst_nullInsn();
                Break;

            Case(RCONST_NULL):
                rconst_nullInsn();
                Break;

            Case(CALL):
                callInsn(wide);
                Break;

            Case(RETURN):
                returnInsn();
                Break;

            Case(IRETURN):
                ireturnInsn();
                Break;

            Case(LRETURN):
                lreturnInsn();
                Break;

            Case(HRETURN):
                hreturnInsn();
                Break;

            Case(RRETURN):
                rreturnInsn();
                Break;

            Case(LDC):
                ldcInsn(wide);
                Break;

            Case(WIDE):
                executeInstruction(true);
                Break;

            Case(BREAKPOINT):
            Case(RESERVE1):
            Case(RESERVE2):
                Break; // these are official instructions that are meant for debuggers or other special vm stuff

			Default:
				std::cout << "Unknown instruction: " << static_cast<i32>(instruction) << ". TODO: proper error stuff\n";
				std::exit(1);
		}
	}

	void Executor::enterFunction(Function* function) {
		mFrame = mStack.enterFrame(function->getLocalCount(), function->getStackSize(), function, mCode, mPC);
        mPC = 0;
		mCode = function->getEntry();
        mReturnDepth++;
	}
	
	u8 Executor::getByte() {
		u8 value = ReadU8(&mCode[mPC], 0);
		mPC += 1;

		return value;
	}
	
	u16 Executor::getShort() {
		u16 value = ReadU16(&mCode[mPC], 0);
		mPC += 2;

		return value;
	}
	
	u32 Executor::getInt() {
		u32 value = ReadU32(&mCode[mPC], 0);
		mPC += 4;

		return value;
	}
	
	u64 Executor::getLong() {
		u64 value = ReadU64(&mCode[mPC], 0);
		mPC += 8;

		return value;
	}


	void Executor::addInsn() {
		i32 rhs = mFrame->pop();
		i32 lhs = mFrame->pop();

		mFrame->push(lhs + rhs);
	}

	void Executor::subInsn() {
		i32 rhs = mFrame->pop();
		i32 lhs = mFrame->pop();

		mFrame->push(lhs - rhs);
	}

	void Executor::mulInsn() {
		i32 rhs = mFrame->pop();
		i32 lhs = mFrame->pop();

		mFrame->push(lhs * rhs);
	}

	void Executor::divInsn() {
		i32 rhs = mFrame->pop();
		i32 lhs = mFrame->pop();

		mFrame->push(lhs / rhs);
	}
	
	void Executor::remInsn() {
		i32 rhs = mFrame->pop();
		i32 lhs = mFrame->pop();

		mFrame->push(lhs - (lhs / rhs) * rhs);
	}
	
	void Executor::laddInsn() {
		i64 rhs = mFrame->popLong();
		i64 lhs = mFrame->popLong();

		mFrame->pushLong(lhs + rhs);
	}
	
	void Executor::lsubInsn() {
		i64 rhs = mFrame->popLong();
		i64 lhs = mFrame->popLong();

		mFrame->pushLong(lhs - rhs);
	}
	
	void Executor::lmulInsn() {
		i64 rhs = mFrame->popLong();
		i64 lhs = mFrame->popLong();

		mFrame->pushLong(lhs * rhs);
	}
	
	void Executor::ldivInsn() {
		i64 rhs = mFrame->popLong();
		i64 lhs = mFrame->popLong();

		mFrame->pushLong(lhs / rhs);
	}
	
	void Executor::lremInsn() {
		i64 rhs = mFrame->popLong();
		i64 lhs = mFrame->popLong();

		mFrame->pushLong(lhs - (lhs / rhs) * rhs);
	}

	void Executor::andInsn() {
		i32 rhs = mFrame->pop();
		i32 lhs = mFrame->pop();

		mFrame->push(lhs & rhs);
	}
	
	void Executor::orInsn() {
		i32 rhs = mFrame->pop();
		i32 lhs = mFrame->pop();

		mFrame->push(lhs | rhs);
	}
	
	void Executor::xorInsn() {
		i32 rhs = mFrame->pop();
		i32 lhs = mFrame->pop();

		mFrame->push(lhs ^ rhs);
	}

    void Executor::shlInsn() {
        i32 rhs = mFrame->pop();
        i32 lhs = mFrame->pop();

        mFrame->push(lhs << rhs);
    }

    void Executor::shrInsn() {
        i32 rhs = mFrame->pop();
        i32 lhs = mFrame->pop();

        mFrame->push(lhs >> rhs);
    }

	void Executor::landInsn() {
		i64 rhs = mFrame->popLong();
		i64 lhs = mFrame->popLong();

		mFrame->pushLong(lhs & rhs);
	}
	
	void Executor::lorInsn() {
		i64 rhs = mFrame->popLong();
		i64 lhs = mFrame->popLong();

		mFrame->pushLong(lhs | rhs);
	}
	
	void Executor::lxorInsn() {
		i64 rhs = mFrame->popLong();
		i64 lhs = mFrame->popLong();

		mFrame->pushLong(lhs ^ rhs);
	}

    void Executor::lshlInsn() {
        i64 rhs = mFrame->popLong();
        i64 lhs = mFrame->popLong();

        mFrame->pushLong(lhs << rhs);
    }

    void Executor::lshrInsn() {
        i64 rhs = mFrame->popLong();
        i64 lhs = mFrame->popLong();

        mFrame->pushLong(lhs >> rhs);
    }

	void Executor::notInsn() {
		i32 value = mFrame->pop();
		mFrame->push(~value);
	}
	
	void Executor::negInsn() {
		i32 value = mFrame->pop();
		mFrame->push(-value);
	}

	void Executor::lnotInsn() {
		i32 value = mFrame->popLong();
		mFrame->pushLong(~value);
	}
	
	void Executor::lnegInsn() {
		i32 value = mFrame->popLong();
		mFrame->pushLong(-value);
	}

    void Executor::popInsn() {
        mFrame->popValue();
    }

    void Executor::dupInsn() {
        mFrame->dup();
    }

    void Executor::dup2Insn() {
        mFrame->dup2();
    }

    void Executor::swapInsn() {
        mFrame->swap();
    }

    void Executor::swap2Insn() {
        mFrame->swap2();
    }

    void Executor::iloadInsn(bool wide) {
        u16 index = wide ? getShort() : getByte();

        mFrame->push(mFrame->getLocalInt(index));
    }

    void Executor::istoreInsn(bool wide) {
        u16 index = wide ? getShort() : getByte();

        auto value = mFrame->pop();
        mFrame->setLocalInt(index, value);
    }

    void Executor::lloadInsn(bool wide) {
        u16 index = wide ? getShort() : getByte();

        mFrame->pushLong(mFrame->getLocalLong(index));
    }

    void Executor::lstoreInsn(bool wide) {
        u16 index = wide ? getShort() : getByte();

        auto value = mFrame->popLong();
        mFrame->setLocalLong(index, value);
    }

    void Executor::hloadInsn(bool wide) {
        u16 index = wide ? getShort() : getByte();

        mFrame->pushHandle(mFrame->getLocalHandle(index));
    }

    void Executor::hstoreInsn(bool wide) {
        u16 index = wide ? getShort() : getByte();

        auto value = mFrame->popHandle();
        mFrame->setLocalHandle(index, value);
    }

    void Executor::rloadInsn(bool wide) {
        u16 index = wide ? getShort() : getByte();

        mFrame->pushObject(mFrame->getLocalObjectWeak(index));
    }

    void Executor::rstoreInsn(bool wide) {
        u16 index = wide ? getShort() : getByte();

        auto value = mFrame->popObject();
        mFrame->setLocalObject(index, value);
    }

    void Executor::rload_0Insn() {
        mFrame->pushObject(mFrame->getLocalObjectWeak(0));
    }

    void Executor::baloadInsn() {
        Int index = mFrame->pop();
        ObjectRef ref = mFrame->popObject();

        ref->nullCheck();

        if (ref->getClass()->getKind() != ClassKind::ARRAY) {
            std::cout << "baload: reference is not an array reference\n";
            std::exit(1);
        }

        if (index < 0 || index >= ref->getArrayLength()) {
            std::cout << "baload: index out of bounds\n";
            std::exit(1);
        }

        // TODO: check if the ref is a byte array

        auto array = ref->getArrayElements<Byte>();

        mFrame->push(array[index]);
    }

    void Executor::bastoreInsn() {
        auto value = static_cast<Byte>(mFrame->pop());
        Int index = mFrame->pop();
        ObjectRef ref = mFrame->popObject();

        ref->nullCheck();

        if (ref->getClass()->getKind() != ClassKind::ARRAY) {
            std::cout << "bastore: reference is not an array reference\n";
            std::exit(1);
        }

        if (index < 0 || index >= ref->getArrayLength()) {
            std::cout << "bastore: index out of bounds\n";
            std::exit(1);
        }

        // TODO: check if the ref is a byte array

        auto array = ref->getArrayElements<Byte>();

        array[index] = value;
    }

    void Executor::caloadInsn() {
        Int index = mFrame->pop();
        ObjectRef ref = mFrame->popObject();

        ref->nullCheck();

        if (ref->getClass()->getKind() != ClassKind::ARRAY) {
            std::cout << "caload: reference is not an array reference\n";
            std::exit(1);
        }

        if (index < 0 || index >= ref->getArrayLength()) {
            std::cout << "caload: index out of bounds\n";
            std::exit(1);
        }

        // TODO: check if the ref is a char array

        auto array = ref->getArrayElements<Char>();

        mFrame->push(array[index]);
    }

    void Executor::castoreInsn() {
        auto value = static_cast<Char>(mFrame->pop());
        Int index = mFrame->pop();
        ObjectRef ref = mFrame->popObject();

        ref->nullCheck();

        if (ref->getClass()->getKind() != ClassKind::ARRAY) {
            std::cout << "castore: reference is not an array reference\n";
            std::exit(1);
        }

        if (index < 0 || index >= ref->getArrayLength()) {
            std::cout << "castore: index out of bounds\n";
            std::exit(1);
        }

        // TODO: check if the ref is a byte array

        auto array = ref->getArrayElements<Char>();

        array[index] = value;
    }

    void Executor::saloadInsn() {
        Int index = mFrame->pop();
        ObjectRef ref = mFrame->popObject();

        ref->nullCheck();

        if (ref->getClass()->getKind() != ClassKind::ARRAY) {
            std::cout << "saload: reference is not an array reference\n";
            std::exit(1);
        }

        if (index < 0 || index >= ref->getArrayLength()) {
            std::cout << "saload: index out of bounds\n";
            std::exit(1);
        }

        // TODO: check if the ref is a short array

        auto array = ref->getArrayElements<Short>();

        mFrame->push(array[index]);
    }

    void Executor::sastoreInsn() {
        auto value = static_cast<Short>(mFrame->pop());
        Int index = mFrame->pop();
        ObjectRef ref = mFrame->popObject();

        ref->nullCheck();

        if (ref->getClass()->getKind() != ClassKind::ARRAY) {
            std::cout << "sastore: reference is not an array reference\n";
            std::exit(1);
        }

        if (index < 0 || index >= ref->getArrayLength()) {
            std::cout << "sastore: index out of bounds\n";
            std::exit(1);
        }

        // TODO: check if the ref is a short array

        auto array = ref->getArrayElements<Short>();

        array[index] = value;
    }

    void Executor::ialoadInsn() {
        Int index = mFrame->pop();
        ObjectRef ref = mFrame->popObject();

        ref->nullCheck();

        if (ref->getClass()->getKind() != ClassKind::ARRAY) {
            std::cout << "iaload: reference is not an array reference\n";
            std::exit(1);
        }

        if (index < 0 || index >= ref->getArrayLength()) {
            std::cout << "iaload: index out of bounds\n";
            std::exit(1);
        }

        // TODO: check if the ref is a int array

        auto array = ref->getArrayElements<Int>();

        mFrame->push(array[index]);
    }

    void Executor::iastoreInsn() {
        auto value = static_cast<Int>(mFrame->pop());
        Int index = mFrame->pop();
        ObjectRef ref = mFrame->popObject();

        ref->nullCheck();

        if (ref->getClass()->getKind() != ClassKind::ARRAY) {
            std::cout << "iastore: reference is not an array reference\n";
            std::exit(1);
        }

        if (index < 0 || index >= ref->getArrayLength()) {
            std::cout << "iastore: index out of bounds\n";
            std::exit(1);
        }

        // TODO: check if the ref is a int array

        auto array = ref->getArrayElements<Int>();

        array[index] = value;
    }

    void Executor::laloadInsn() {
        Int index = mFrame->pop();
        ObjectRef ref = mFrame->popObject();

        ref->nullCheck();

        if (ref->getClass()->getKind() != ClassKind::ARRAY) {
            std::cout << "laload: reference is not an array reference\n";
            std::exit(1);
        }

        if (index < 0 || index >= ref->getArrayLength()) {
            std::cout << "laload: index out of bounds\n";
            std::exit(1);
        }

        // TODO: check if the ref is a long array

        auto array = ref->getArrayElements<Long>();

        mFrame->pushLong(array[index]);
    }

    void Executor::lastoreInsn() {
        Long value = mFrame->popLong();
        Int index = mFrame->pop();
        ObjectRef ref = mFrame->popObject();

        ref->nullCheck();

        if (ref->getClass()->getKind() != ClassKind::ARRAY) {
            std::cout << "lastore: reference is not an array reference\n";
            std::exit(1);
        }

        if (index < 0 || index >= ref->getArrayLength()) {
            std::cout << "lastore: index out of bounds\n";
            std::exit(1);
        }

        // TODO: check if the ref is a long array

        auto array = ref->getArrayElements<Long>();

        array[index] = value;
    }

    void Executor::haloadInsn() {
        Int index = mFrame->pop();
        ObjectRef ref = mFrame->popObject();

        ref->nullCheck();

        if (ref->getClass()->getKind() != ClassKind::ARRAY) {
            std::cout << "haload: reference is not an array reference\n";
            std::exit(1);
        }

        if (index < 0 || index >= ref->getArrayLength()) {
            std::cout << "haload: index out of bounds\n";
            std::exit(1);
        }

        // TODO: check if the ref is a handle array

        auto array = ref->getArrayElements<Handle>();

        mFrame->pushHandle(array[index]);
    }

    void Executor::hastoreInsn() {
        Handle value = mFrame->popHandle();
        Int index = mFrame->pop();
        ObjectRef ref = mFrame->popObject();

        ref->nullCheck();

        if (ref->getClass()->getKind() != ClassKind::ARRAY) {
            std::cout << "hastore: reference is not an array reference\n";
            std::exit(1);
        }

        if (index < 0 || index >= ref->getArrayLength()) {
            std::cout << "hastore: index out of bounds\n";
            std::exit(1);
        }

        // TODO: check if the ref is a handle array

        auto array = ref->getArrayElements<Handle>();

        array[index] = value;
    }

    void Executor::raloadInsn() {
        Int index = mFrame->pop();
        ObjectRef ref = mFrame->popObject();

        ref->nullCheck();

        if (ref->getClass()->getKind() != ClassKind::ARRAY) {
            std::cout << "raload: reference is not an array reference\n";
            std::exit(1);
        }

        if (index < 0 || index >= ref->getArrayLength()) {
            std::cout << "raload: index out of bounds\n";
            std::exit(1);
        }

        // TODO: check if the ref is a char array

        auto array = ref->getArrayElements<Object*>();

        mFrame->pushObject(array[index]);
    }

    void Executor::rastoreInsn() {
        ObjectRef value = mFrame->popObject();
        Int index = mFrame->pop();
        ObjectRef ref = mFrame->popObject();

        ref->nullCheck();

        if (ref->getClass()->getKind() != ClassKind::ARRAY) {
            std::cout << "rastore: reference is not an array reference\n";
            std::exit(1);
        }

        if (index < 0 || index >= ref->getArrayLength()) {
            std::cout << "rastore: index out of bounds\n";
            std::exit(1);
        }

        // TODO: check if the ref is a reference array

        auto array = ref->getArrayElements<Object*>();

        array[index] = value;
        value->addReference(); // the array gets its reference here, we lose our reference right after from destructor
    }

    void Executor::arraylengthInsn() {
        ObjectRef ref = mFrame->popObject();

        ref->nullCheck();

        if (ref->getClass()->getKind() != ClassKind::ARRAY) {
            std::cout << "arraylength: reference is not an array reference\n";
            std::exit(1);
        }

        mFrame->push(ref->getArrayLength());
    }

    void Executor::newInsn(bool wide) {
        u32 index = wide ? getInt() : getShort();
        auto classRef = mFrame->getConstPool().get<ConstantClass>(index);

        if (classRef == nullptr) {
            std::cout << "constant index out of bounds. todo: better errors\n";
            std::exit(1);
        }

        ObjectRef obj = AllocObject(classRef->getClass()); // we get our reference from the alloc call

        mFrame->pushObject(obj);
    }

    void Executor::newarrayInsn() {
        u8 typeId = getByte();
        Int size = mFrame->pop();

        ObjectRef array = AllocPrimitiveArray(typeId, size);

        mFrame->pushObject(array);
    }

    void Executor::rnewarrayInsn(bool wide) {
        u32 index = wide ? getInt() : getShort();
        auto classRef = mFrame->getConstPool().get<ConstantClass>(index);

        if (classRef == nullptr) {
            std::cout << "constant index out of bounds. todo: better errors\n";
            std::exit(1);
        }

        Int size = mFrame->pop();

        ObjectRef array = AllocArrayOf(classRef->getClass(), size);

        mFrame->pushObject(array);
    }

    void Executor::isinstanceInsn(bool wide) {
        u32 index = wide ? getInt() : getShort();
        auto classRef = mFrame->getConstPool().get<ConstantClass>(index);

        if (classRef == nullptr) {
            std::cout << "constant index out of bounds. todo: better errors\n";
            std::exit(1);
        }

        ObjectRef object = mFrame->popObject();
        object->nullCheck();

        mFrame->push(object->isInstance(classRef->getClass()));
    }

    void Executor::getfieldInsn(bool wide) {
        u32 index = wide ? getInt() : getShort();
        auto fieldRef = mFrame->getConstPool().get<ConstantField>(index);

        if (fieldRef == nullptr) {
            std::cout << "constant index out of bounds. todo: better errors\n";
            std::exit(1);
        }

        ObjectRef object = mFrame->popObject();

        Field* field = fieldRef->getField();

        if (!object->isInstance(field->getClass())) {
            std::cout << "error: field '" << field->getType().getClassName() << " " << field->getName() << "' is not a part of the class " <<
            object->getClass()->getModule()->getName() << ":" << object->getClass()->getName() << "\n";
            std::exit(1);
        }

        object->nullCheck();

        switch (field->getType().getInternalType()) {
            case Type::REFERENCE:
                mFrame->pushObject(object->getObject(field));
                Break;
            case Type::HANDLE:
                mFrame->pushHandle(object->getHandle(field));
                Break;
            case Type::BYTE:
                mFrame->push(object->getByte(field));
                Break;
            case Type::SHORT:
                mFrame->push(object->getShort(field));
                Break;
            case Type::INT:
                mFrame->push(object->getInt(field));
                Break;
            case Type::LONG:
                mFrame->pushLong(object->getLong(field));
                Break;
            case Type::CHAR:
                mFrame->push(object->getChar(field));
                Break;
            case Type::FLOAT:
                std::cout << "float is NOT supported yet\n";
                std::exit(1);
            case Type::DOUBLE:
                std::cout << "double is NOT supported yet\n";
                std::exit(1);
            case Type::BOOL:
                mFrame->push(object->getBool(field));
                Break;
        }
    }

    void Executor::setfieldInsn(bool wide) {
        u32 index = wide ? getInt() : getShort();
        auto fieldRef = mFrame->getConstPool().get<ConstantField>(index);

        if (fieldRef == nullptr) {
            std::cout << "constant index out of bounds. todo: better errors\n";
            std::exit(1);
        }

        Field* field = fieldRef->getField();

        switch (field->getType().getInternalType()) {
            case Type::REFERENCE: {
                ObjectRef value = mFrame->popObject();
                ObjectRef object = mFrame->popObject();

                object->nullCheck();

                if (!object->isInstance(field->getClass())) {
                    std::cout << "error: field '" << field->getType().getClassName() << " " << field->getName() << "' is not a part of the class " <<
                              object->getClass()->getModule()->getName() << ":" << object->getClass()->getName() << "\n";
                    std::exit(1);
                }

                object->setObject(field, value);
                Break;
            }
            case Type::HANDLE: {
                Handle value = mFrame->popHandle();
                ObjectRef object = mFrame->popObject();

                object->nullCheck();

                if (!object->isInstance(field->getClass())) {
                    std::cout << "error: field '" << field->getType().getClassName() << " " << field->getName() << "' is not a part of the class " <<
                              object->getClass()->getModule()->getName() << ":" << object->getClass()->getName() << "\n";
                    std::exit(1);
                }

                object->setHandle(field, value);
                Break;
            }
            case Type::BYTE: {
                auto value = static_cast<Byte>(mFrame->pop());
                ObjectRef object = mFrame->popObject();

                object->nullCheck();

                if (!object->isInstance(field->getClass())) {
                    std::cout << "error: field '" << field->getType().getClassName() << " " << field->getName() << "' is not a part of the class " <<
                              object->getClass()->getModule()->getName() << ":" << object->getClass()->getName() << "\n";
                    std::exit(1);
                }

                object->setByte(field, value);
                Break;
            }
            case Type::SHORT: {
                auto value = static_cast<Short>(mFrame->pop());
                ObjectRef object = mFrame->popObject();

                object->nullCheck();

                if (!object->isInstance(field->getClass())) {
                    std::cout << "error: field '" << field->getType().getClassName() << " " << field->getName() << "' is not a part of the class " <<
                              object->getClass()->getModule()->getName() << ":" << object->getClass()->getName() << "\n";
                    std::exit(1);
                }

                object->setShort(field, value);
                Break;
            }
            case Type::INT: {
                Int value = mFrame->pop();
                ObjectRef object = mFrame->popObject();

                object->nullCheck();

                if (!object->isInstance(field->getClass())) {
                    std::cout << "error: field '" << field->getType().getClassName() << " " << field->getName() << "' is not a part of the class " <<
                              object->getClass()->getModule()->getName() << ":" << object->getClass()->getName() << "\n";
                    std::exit(1);
                }

                object->setInt(field, value);
                Break;
            }
            case Type::LONG: {
                Long value = mFrame->popLong();
                ObjectRef object = mFrame->popObject();

                object->nullCheck();

                if (!object->isInstance(field->getClass())) {
                    std::cout << "error: field '" << field->getType().getClassName() << " " << field->getName() << "' is not a part of the class " <<
                              object->getClass()->getModule()->getName() << ":" << object->getClass()->getName() << "\n";
                    std::exit(1);
                }

                object->setLong(field, value);
                Break;
            }
            case Type::CHAR: {
                auto value = static_cast<Char>(mFrame->pop());
                ObjectRef object = mFrame->popObject();

                object->nullCheck();

                if (!object->isInstance(field->getClass())) {
                    std::cout << "error: field '" << field->getType().getClassName() << " " << field->getName() << "' is not a part of the class " <<
                              object->getClass()->getModule()->getName() << ":" << object->getClass()->getName() << "\n";
                    std::exit(1);
                }

                object->setChar(field, value);
                Break;
            }
            case Type::FLOAT:
                std::cout << "float is NOT supported yet\n";
                std::exit(1);
            case Type::DOUBLE:
                std::cout << "double is NOT supported yet\n";
                std::exit(1);
            case Type::BOOL: {
                auto value = static_cast<Bool>(mFrame->pop());
                ObjectRef object = mFrame->popObject();

                object->nullCheck();

                if (!object->isInstance(field->getClass())) {
                    std::cout << "error: field '" << field->getType().getClassName() << " " << field->getName() << "' is not a part of the class " <<
                              object->getClass()->getModule()->getName() << ":" << object->getClass()->getName() << "\n";
                    std::exit(1);
                }

                object->setBool(field, value);
                Break;
            }
        }
    }

    void Executor::getglobalInsn(bool wide) {
        u32 index = wide ? getInt() : getShort();
        auto globalRef = mFrame->getConstPool().get<ConstantGlobalVar>(index);

        if (globalRef == nullptr) {
            std::cout << "constant index out of bounds. todo: better errors\n";
            std::exit(1);
        }

        GlobalVar* global = globalRef->getGlobalVar();

        switch (global->getType().getInternalType()) {
            case Type::REFERENCE:
                mFrame->pushObject(reinterpret_cast<Object*>(global->getValue().R));
                Break;
            case Type::HANDLE:
                mFrame->pushHandle(global->getValue().H);
                Break;
            case Type::BYTE:
                mFrame->push(global->getValue().B);
                Break;
            case Type::SHORT:
                mFrame->push(global->getValue().S);
                Break;
            case Type::INT:
                mFrame->push(global->getValue().I);
                Break;
            case Type::LONG:
                mFrame->pushLong(global->getValue().L);
                Break;
            case Type::CHAR:
                mFrame->push(global->getValue().C);
                Break;
            case Type::FLOAT:
                mFrame->push(global->getValue().F);
                Break;
            case Type::DOUBLE:
                mFrame->push(global->getValue().D);
                Break;
            case Type::BOOL:
                mFrame->push(global->getValue().Z);
                Break;
        }
    }

    void Executor::setglobalInsn(bool wide) {
        u32 index = wide ? getInt() : getShort();
        auto globalRef = mFrame->getConstPool().get<ConstantGlobalVar>(index);

        if (globalRef == nullptr) {
            std::cout << "constant index out of bounds. todo: better errors\n";
            std::exit(1);
        }

        GlobalVar* global = globalRef->getGlobalVar();

        switch (global->getType().getInternalType()) {
            case Type::REFERENCE: {
                ObjectRef object = mFrame->popObject();
                global->setValue(object);

                if (object != nullptr) object->addReference();
                Break;
            }
            case Type::HANDLE: {
                Handle handle = mFrame->popHandle();
                global->setValue(handle);

                Break;
            }
            case Type::BYTE: {
                auto value = static_cast<Byte>(mFrame->pop());
                global->setValue(value);

                Break;
            }
            case Type::SHORT: {
                auto value = static_cast<Short>(mFrame->pop());
                global->setValue(value);

                Break;
            }
            case Type::INT: {
                auto value = static_cast<Int>(mFrame->pop());
                global->setValue(value);

                Break;
            }
            case Type::LONG: {
                Long value = mFrame->popLong();
                global->setValue(value);

                Break;
            }
            case Type::CHAR: {
                auto value = static_cast<Char>(mFrame->pop());
                global->setValue(value);

                Break;
            }
            case Type::FLOAT: {
                auto value = static_cast<Float>(mFrame->pop());
                global->setValue(value);

                Break;
            }
            case Type::DOUBLE: {
                auto value = static_cast<Double >(mFrame->popLong());
                global->setValue(value);

                Break;
            }
            case Type::BOOL: {
                auto value = static_cast<Bool>(mFrame->pop());
                global->setValue(value);

                Break;
            }
        }
    }

	void Executor::jmp_icmpeqInsn() {
		i16 branch = getShort(); // relative

		i32 rhs = mFrame->pop();
		i32 lhs = mFrame->pop();

		if (lhs == rhs) {
			mPC += branch;
		}
	}
	
	void Executor::jmp_icmpneInsn() {
		i16 branch = getShort(); // relative

		i32 rhs = mFrame->pop();
		i32 lhs = mFrame->pop();

		if (lhs != rhs) {
			mPC += branch;
		}
	}
	
	void Executor::jmp_icmpltInsn() {
		i16 branch = getShort(); // relative

		i32 rhs = mFrame->pop();
		i32 lhs = mFrame->pop();

		if (lhs < rhs) {
			mPC += branch;
		}
	}
	
	void Executor::jmp_icmpgtInsn() {
		i16 branch = getShort(); // relative

		i32 rhs = mFrame->pop();
		i32 lhs = mFrame->pop();

		if (lhs > rhs) {
			mPC += branch;
		}
	}
	
	void Executor::jmp_icmpleInsn() {
		i16 branch = getShort(); // relative

		i32 rhs = mFrame->pop();
		i32 lhs = mFrame->pop();

		if (lhs <= rhs) {
			mPC += branch;
		}
	}
	
	void Executor::jmp_icmpgeInsn() {
		i16 branch = getShort(); // relative

		i32 rhs = mFrame->pop();
		i32 lhs = mFrame->pop();

		if (lhs >= rhs) {
			mPC += branch;
		}
	}


    void Executor::jmp_hcmpeqInsn() {
        i16 branch = getShort(); // relative

        Handle rhs = mFrame->popHandle();
        Handle lhs = mFrame->popHandle();

        if (lhs == rhs) {
            mPC += branch;
        }
    }

    void Executor::jmp_hcmpneInsn() {
        i16 branch = getShort(); // relative

        Handle rhs = mFrame->popHandle();
        Handle lhs = mFrame->popHandle();

        if (lhs != rhs) {
            mPC += branch;
        }
    }

    void Executor::jmp_rcmpeqInsn() {
        i16 branch = getShort(); // relative

        ObjectRef rhs = mFrame->popObject();
        ObjectRef lhs = mFrame->popObject();

        if (lhs == rhs) {
            mPC += branch;
        }
    }

    void Executor::jmp_rcmpneInsn() {
        i16 branch = getShort(); // relative

        ObjectRef rhs = mFrame->popObject();
        ObjectRef lhs = mFrame->popObject();

        if (lhs != rhs) {
            mPC += branch;
        }
    }

    void Executor::jmp_hnullInsn() {
        i16 branch = getShort(); // relative

        Handle value = mFrame->popHandle();

        if (value == nullptr) {
            mPC += branch;
        }
    }

    void Executor::jmp_hnonnullInsn() {
        i16 branch = getShort(); // relative

        Handle value = mFrame->popHandle();

        if (value != nullptr) {
            mPC += branch;
        }
    }

    void Executor::jmp_rnullInsn() {
        i16 branch = getShort(); // relative

        ObjectRef value = mFrame->popObject();

        if (value == nullptr) {
            mPC += branch;
        }
    }

    void Executor::jmp_rnonnullInsn() {
        i16 branch = getShort(); // relative

        ObjectRef value = mFrame->popObject();

        if (value != nullptr) {
            mPC += branch;
        }
    }

	void Executor::jmpeqInsn() {
		i16 branch = getShort(); // relative

		i32 value = mFrame->pop();

		if (value == 0) {
			mPC += branch;
		}
	}
	
	void Executor::jmpneInsn() {
		i16 branch = getShort(); // relative

		i32 value = mFrame->pop();

		if (value != 0) {
			mPC += branch;
		}
	}
	
	void Executor::jmpltInsn() {
		i16 branch = getShort(); // relative

		i32 value = mFrame->pop();

		if (value < 0) {
			mPC += branch;
		}
	}
	
	void Executor::jmpgtInsn() {
		i16 branch = getShort(); // relative

		i32 value = mFrame->pop();

		if (value > 0) {
			mPC += branch;
		}
	}
	
	void Executor::jmpleInsn() {
		i16 branch = getShort(); // relative

		i32 value = mFrame->pop();

		if (value <= 0) {
			mPC += branch;
		}
	}
	
	void Executor::jmpgeInsn() {
		i16 branch = getShort(); // relative

		i32 value = mFrame->pop();

		if (value >= 0) {
			mPC += branch;
		}
	}

    void Executor::jmpInsn() {
        i16 branch = getShort();
        mPC += branch;
    }

	void Executor::icmpInsn() {
		i32 rhs = mFrame->pop();
		i32 lhs = mFrame->pop();

		if (lhs < rhs) mFrame->push(-1);
		else if (lhs > rhs) mFrame->push(1);
		else mFrame->push(0);
	}
	
	void Executor::lcmpInsn() {
		i64 rhs = mFrame->popLong();
		i64 lhs = mFrame->popLong();

		if (lhs < rhs) mFrame->push(-1);
		else if (lhs > rhs) mFrame->push(1);
		else mFrame->push(0);
	}

    void Executor::hcmpInsn() {
        Handle rhs = mFrame->popHandle();
        Handle lhs = mFrame->popHandle();

        if (lhs < rhs) mFrame->push(-1);
        else if (lhs > rhs) mFrame->push(1);
        else mFrame->push(0);
    }

    void Executor::rcmpInsn() {
        ObjectRef rhs = mFrame->popObject();
        ObjectRef lhs = mFrame->popObject();

        if (lhs.get() < rhs.get()) mFrame->push(-1);
        else if (lhs.get() > rhs.get()) mFrame->push(1);
        else mFrame->push(0);
    }

	void Executor::bpushInsn() {
		i8 value = getByte();
		mFrame->push(value);
	}

    void Executor::spushInsn() {
        i16 value = getShort();
        mFrame->push(value);
    }

    void Executor::ipushInsn() {
        i32 value = getInt();
        mFrame->push(value);
    }

    void Executor::lpushInsn() {
        i64 value = getLong();
        mFrame->pushLong(value);
    }

    void Executor::i2bInsn() {
        Int value = mFrame->pop();
        mFrame->push(static_cast<Byte>(value));
    }

    void Executor::i2sInsn() {
        Int value = mFrame->pop();
        mFrame->push(static_cast<Short>(value));
    }

    void Executor::i2lInsn() {
        Int value = mFrame->pop();
        mFrame->pushLong(value);
    }

    void Executor::l2iInsn() {
        Long value = mFrame->popLong();
        mFrame->push(static_cast<Int>(value));
    }

    void Executor::constInsn(i32 value) {
        mFrame->push(value);
    }

    void Executor::lconstInsn(i64 value) {
        mFrame->pushLong(value);
    }

    void Executor::hconst_nullInsn() {
        mFrame->pushHandle(nullptr);
    }

    void Executor::rconst_nullInsn() {
        mFrame->pushObject(nullptr);
    }

    void Executor::callInsn(bool wide) {
        u32 index = wide ? getInt() : getShort();
        auto funcRef = mFrame->getConstPool().get<ConstantFunc>(index);

        if (funcRef == nullptr) {
            std::cout << "constant index out of bounds. todo: better errors\n";
            std::exit(1);
        }

        Function* func = funcRef->getFunction();

        if (func->isAsync()) {
            Threading::Schedule(func);
            return;
        }

        if (func->isNative()) {
            if (func->getEntry() == nullptr) {
                Linker::LinkNativeFunction(func);
            }

            std::unique_ptr<JValue[]> nativeArgs = std::make_unique<JValue[]>(func->getArgumentTypes().size());

            u16 i = func->getArgumentTypes().size();

            for (const auto& arg : std::ranges::reverse_view(func->getArgumentTypes())) {
                switch (arg.getInternalType()) {
                    case Type::REFERENCE: {
                        ObjectRef obj = mFrame->popObject();

                        i -= 1;
                        nativeArgs[i].R = obj;
                        obj->addReference(); // the native arg needs its own explicit reference here

                        Break;
                    }

                    case Type::HANDLE: {
                        auto value = mFrame->popHandle();

                        i -= 1;
                        nativeArgs[i].H = value;

                        Break;
                    }

                    case Type::BYTE: {
                        auto value = mFrame->pop();

                        i -= 1;
                        nativeArgs[i].B = static_cast<Byte>(value);

                        Break;
                    }

                    case Type::SHORT: {
                        auto value = mFrame->pop();

                        i -= 1;
                        nativeArgs[i].S = static_cast<Short>(value);

                        Break;
                    }

                    case Type::INT: {
                        auto value = mFrame->pop();

                        i -= 1;
                        nativeArgs[i].I = static_cast<Int>(value);

                        Break;
                    }

                    case Type::LONG: {
                        auto value = mFrame->popLong();

                        i -= 1;
                        nativeArgs[i].L = static_cast<Long>(value);

                        Break;
                    }

                    case Type::CHAR: {
                        auto value = mFrame->pop();

                        i -= 1;
                        nativeArgs[i].C = static_cast<Char>(value);

                        Break;
                    }

                    case Type::FLOAT:
                    case Type::DOUBLE:
                        std::exit(3);
                    case Type::BOOL: {
                        auto value = mFrame->pop();

                        i -= 1;
                        nativeArgs[i].Z = (value != 0);

                        Break;
                    }
                }
            }

            if (func->getReturnType().getInternalType() == Type::VOID) {
                auto ptr = reinterpret_cast<NativeFunctionPtr<void>>(func->getEntry());
                ptr(GetContext(), nativeArgs.get());
            } else {
                switch (func->getReturnType().getInternalType()) {
                    case Type::REFERENCE: {
                        auto ptr = reinterpret_cast<NativeFunctionPtr<JObject>>(func->getEntry());
                        auto value = ptr(GetContext(), nativeArgs.get());
                        mFrame->pushObject(reinterpret_cast<Object*>(value));
                        Break;
                    }

                    case Type::HANDLE: {
                        auto ptr = reinterpret_cast<NativeFunctionPtr<Handle>>(func->getEntry());
                        auto value = ptr(GetContext(), nativeArgs.get());
                        mFrame->pushHandle(value);
                        Break;
                    }

                    case Type::BYTE: {
                        auto ptr = reinterpret_cast<NativeFunctionPtr<Byte>>(func->getEntry());
                        auto value = ptr(GetContext(), nativeArgs.get());
                        mFrame->push(value);
                        Break;
                    }

                    case Type::SHORT: {
                        auto ptr = reinterpret_cast<NativeFunctionPtr<Short>>(func->getEntry());
                        auto value = ptr(GetContext(), nativeArgs.get());
                        mFrame->push(value);
                        Break;
                    }

                    case Type::INT: {
                        auto ptr = reinterpret_cast<NativeFunctionPtr<Int>>(func->getEntry());
                        auto value = ptr(GetContext(), nativeArgs.get());
                        mFrame->push(value);
                        Break;
                    }

                    case Type::LONG: {
                        auto ptr = reinterpret_cast<NativeFunctionPtr<Long>>(func->getEntry());
                        auto value = ptr(GetContext(), nativeArgs.get());
                        mFrame->pushLong(value);
                        Break;
                    }

                    case Type::CHAR: {
                        auto ptr = reinterpret_cast<NativeFunctionPtr<Char>>(func->getEntry());
                        auto value = ptr(GetContext(), nativeArgs.get());
                        mFrame->push(value);
                        Break;
                    }

                    case Type::FLOAT:
                    case Type::DOUBLE:
                        std::exit(3);

                    case Type::BOOL:
                        auto ptr = reinterpret_cast<NativeFunctionPtr<Bool>>(func->getEntry());
                        auto value = ptr(GetContext(), nativeArgs.get());
                        mFrame->push(value);
                        Break;
                }
            }

            for (i = 0; i < func->getArgumentTypes().size(); i++) {
                if (func->getArgumentTypes()[i].getInternalType() == Type::REFERENCE) {
                    auto object = reinterpret_cast<Object*>(nativeArgs[i].R);
                    object->removeReference(); // explicitly remove the native reference after call
                }
            }

            return;
        }

        Stack::Frame* oldFrame = mFrame;
        enterFunction(func);

        u16 i = func->getNeededLocalsForArgs();

        for (auto & arg : std::ranges::reverse_view(func->getArgumentTypes())) {
            switch (arg.getInternalType()) {
                case Type::REFERENCE: {
                    ObjectRef obj = oldFrame->popObject();

                    i -= 2;
                    mFrame->setLocalObject(i, obj);

                    Break;
                }

                case Type::HANDLE: {
                    auto value = oldFrame->popHandle();

                    i -= 2;
                    mFrame->setLocalHandle(i, value);

                    Break;
                }

                case Type::BYTE: {
                    auto value = oldFrame->pop();

                    i -= 1;
                    mFrame->setLocalInt(i, static_cast<Byte>(value));

                    Break;
                }

                case Type::SHORT: {
                    auto value = oldFrame->pop();

                    i -= 1;
                    mFrame->setLocalInt(i, static_cast<Short>(value));

                    Break;
                }

                case Type::INT: {
                    auto value = oldFrame->pop();

                    i -= 1;
                    mFrame->setLocalInt(i, static_cast<Int>(value));

                    Break;
                }

                case Type::LONG: {
                    auto value = oldFrame->popLong();

                    i -= 2;
                    mFrame->setLocalLong(i, static_cast<Long>(value));

                    Break;
                }

                case Type::CHAR: {
                    auto value = oldFrame->pop();

                    i -= 1;
                    mFrame->setLocalInt(i, static_cast<Char>(value));

                    Break;
                }

                case Type::FLOAT:
                case Type::DOUBLE:
                    std::exit(3);
                case Type::BOOL: {
                    auto value = oldFrame->pop();

                    i -= 1;
                    mFrame->setLocalInt(i, value != 0);

                    Break;
                }
            }
        }
    }

	void Executor::returnInsn() {
        mPC = mFrame->getReturnPC();
		mCode = mFrame->getReturnCode();
		mFrame = mStack.leaveFrame();

        mReturnDepth--;
	}

    void Executor::ireturnInsn() {
        i32 value = mFrame->pop();

        mPC = mFrame->getReturnPC();
        mCode = mFrame->getReturnCode();
        mFrame = mStack.leaveFrame();

        mReturnDepth--;
        mReturnValue.I = value;

        if (mFrame != nullptr) {
            mFrame->push(value);
        }
    }

    void Executor::lreturnInsn() {
        i64 value = mFrame->popLong();

        mPC = mFrame->getReturnPC();
        mCode = mFrame->getReturnCode();
        mFrame = mStack.leaveFrame();

        mReturnDepth--;
        mReturnValue.L = value;

        if (mFrame != nullptr) {
            mFrame->pushLong(value);
        }
    }

    void Executor::hreturnInsn() {
        Handle value = mFrame->popHandle();

        mPC = mFrame->getReturnPC();
        mCode = mFrame->getReturnCode();
        mFrame = mStack.leaveFrame();

        mReturnDepth--;
        mReturnValue.H = value;

        if (mFrame != nullptr) {
            mFrame->pushHandle(value);
        }
    }

    void Executor::rreturnInsn() {
        ObjectRef value = mFrame->popObject();

        mPC = mFrame->getReturnPC();
        mCode = mFrame->getReturnCode();
        mFrame = mStack.leaveFrame();

        mReturnDepth--;
        mReturnValue.R = value;

        if (mFrame == nullptr) {
            std::cout << "warning: potential memory leak. native interface with objects is not stable yet!\n";
            value->addReference(); // the native value gets a reference
        } else {
            mFrame->pushObject(value);
        }
    }

    void Executor::ldcInsn(bool wide) {
        u32 index = wide ? getInt() : getShort();
        Constant* value = mFrame->getConstPool().getGeneric(index);

        if (value == nullptr) {
            std::cout << "error: index out of bounds\n";
            std::exit(1);
        }

        value->addTo(mFrame);
    }
}