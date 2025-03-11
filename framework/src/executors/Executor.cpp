 #include "JesusVM/JesusVM.h"

#include "JesusVM/bytecode/Opcodes.h"

#include "JesusVM/constpool/ConstantClass.h"
#include "JesusVM/constpool/ConstantField.h"
#include "JesusVM/constpool/ConstantFunc.h"
#include "JesusVM/constpool/ConstPool.h"

#include "JesusVM/executors/Executor.h"
#include "JesusVM/executors/VThread.h"

#undef VOID
#undef BOOL

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

	Executor::Executor(JesusVM& vm)
        : mVM(vm)
		, mFrame(mStack.getTopFrame())
        , mReturned(false)
        , mReturnValue(0)
		, mPC(0)
        , mCode(nullptr) {}

    JValue Executor::getReturnValue() const {
        if (!mReturned) {
            std::cout << "error: illegal state at " << mFrame->getCurrentFunction()->getName() << ": get return value before return"; // illegal state
            std::exit(1);
        }

        return mReturnValue;
    }

    void Executor::run() {
        mReturned = false;

        while (!mReturned) {
            executeInstruction();
        }
    }

	void Executor::executeInstruction(bool wide) {
        if (mReturned || mPC >= mFrame->getCurrentFunction()->getBytecodeSize()) {
            std::cout << "bad execution!!!\n";
            std::exit(1);
        }

		u8 instruction = mCode[mPC++];

		switch (instruction) {
            case Opcodes::NOP:
                break;

			case Opcodes::ADD:
				addInsn();
				break;

			case Opcodes::SUB:
				subInsn();
				break;

            case Opcodes::MUL:
                mulInsn();
                break;

            case Opcodes::DIV:
                divInsn();
                break;

            case Opcodes::REM:
                remInsn();
                break;

            case Opcodes::LADD:
                laddInsn();
                break;

            case Opcodes::LSUB:
                lsubInsn();
                break;

            case Opcodes::LMUL:
                lmulInsn();
                break;

            case Opcodes::LDIV:
                ldivInsn();
                break;

            case Opcodes::LREM:
                lremInsn();
                break;

            case Opcodes::AND:
                andInsn();
                break;

            case Opcodes::OR:
                orInsn();
                break;

            case Opcodes::XOR:
                xorInsn();
                break;

            case Opcodes::SHL:
                shlInsn();
                break;

            case Opcodes::SHR:
                shrInsn();
                break;

            case Opcodes::LAND:
                landInsn();
                break;

            case Opcodes::LOR:
                lorInsn();
                break;

            case Opcodes::LXOR:
                lxorInsn();
                break;

            case Opcodes::LSHL:
                lshlInsn();
                break;

            case Opcodes::LSHR:
                lshrInsn();
                break;

            case Opcodes::NOT:
                notInsn();
                break;

            case Opcodes::NEG:
                negInsn();
                break;

            case Opcodes::LNOT:
                lnotInsn();
                break;

            case Opcodes::LNEG:
                lnegInsn();
                break;

            case Opcodes::DUP:
                dupInsn();
                break;

            case Opcodes::DUP2:
                dup2Insn();
                break;

            case Opcodes::SWAP:
                swapInsn();
                break;

            case Opcodes::SWAP2:
                swap2Insn();
                break;

            case Opcodes::ILOAD:
                iloadInsn(wide);
                break;

            case Opcodes::ISTORE:
                istoreInsn(wide);
                break;

            case Opcodes::LLOAD:
                lloadInsn(wide);
                break;

            case Opcodes::LSTORE:
                lstoreInsn(wide);
                break;

            case Opcodes::HLOAD:
                hloadInsn(wide);
                break;

            case Opcodes::HSTORE:
                hstoreInsn(wide);
                break;

            case Opcodes::RLOAD:
                rloadInsn(wide);
                break;

            case Opcodes::RSTORE:
                rstoreInsn(wide);
                break;

            case Opcodes::RLOAD_0:
                rload_0Insn();
                break;

            case Opcodes::BALOAD:
                baloadInsn();
                break;

            case Opcodes::BASTORE:
                bastoreInsn();
                break;

            case Opcodes::CALOAD:
                caloadInsn();
                break;

            case Opcodes::CASTORE:
                castoreInsn();
                break;

            case Opcodes::SALOAD:
                saloadInsn();
                break;

            case Opcodes::SASTORE:
                sastoreInsn();
                break;

            case Opcodes::IALOAD:
                ialoadInsn();
                break;

            case Opcodes::IASTORE:
                iastoreInsn();
                break;

            case Opcodes::LALOAD:
                laloadInsn();
                break;

            case Opcodes::LASTORE:
                lastoreInsn();
                break;

            case Opcodes::HALOAD:
                haloadInsn();
                break;

            case Opcodes::HASTORE:
                hastoreInsn();
                break;

            case Opcodes::RALOAD:
                raloadInsn();
                break;

            case Opcodes::RASTORE:
                rastoreInsn();
                break;

            case Opcodes::ARRAYLENGTH:
                arraylengthInsn();
                break;

            case Opcodes::NEW:
                newInsn(wide);
                break;

            case Opcodes::NEWARRAY:
                newarrayInsn();
                break;

            case Opcodes::RNEWARRAY:
                rnewarrayInsn(wide);
                break;

            case Opcodes::ISINSTANCE:
                isinstanceInsn(wide);
                break;

            case Opcodes::GETFIELD:
                getfieldInsn(wide);
                break;

            case Opcodes::SETFIELD:
                setfieldInsn(wide);
                break;

            case Opcodes::JMP_ICMPEQ:
                jmp_icmpeqInsn();
                break;

            case Opcodes::JMP_ICMPNE:
                jmp_icmpneInsn();
                break;

            case Opcodes::JMP_ICMPLT:
                jmp_icmpltInsn();
                break;

            case Opcodes::JMP_ICMPGT:
                jmp_icmpgtInsn();
                break;

            case Opcodes::JMP_ICMPLE:
                jmp_icmpleInsn();
                break;

            case Opcodes::JMP_ICMPGE:
                jmp_icmpgeInsn();
                break;

            case Opcodes::JMP_HCMPEQ:
                jmp_hcmpeqInsn();
                break;

            case Opcodes::JMP_HCMPNE:
                jmp_hcmpneInsn();
                break;

            case Opcodes::JMP_RCMPEQ:
                jmp_rcmpeqInsn();
                break;

            case Opcodes::JMP_RCMPNE:
                jmp_rcmpneInsn();
                break;

            case Opcodes::JMP_HNULL:
                jmp_hnullInsn();
                break;

            case Opcodes::JMP_HNONNULL:
                jmp_hnonnullInsn();
                break;

            case Opcodes::JMP_RNULL:
                jmp_rnullInsn();
                break;

            case Opcodes::JMP_RNONNULL:
                jmp_rnonnullInsn();
                break;

            case Opcodes::JMPEQ:
                jmpeqInsn();
                break;

            case Opcodes::JMPNE:
                jmpneInsn();
                break;

            case Opcodes::JMPLT:
                jmpltInsn();
                break;

            case Opcodes::JMPGT:
                jmpgtInsn();
                break;

            case Opcodes::JMPLE:
                jmpleInsn();
                break;

            case Opcodes::JMPGE:
                jmpgeInsn();
                break;

            case Opcodes::JMP:
                jmpInsn();
                break;

            case Opcodes::ICMP:
                icmpInsn();
                break;

            case Opcodes::LCMP:
                lcmpInsn();
                break;

            case Opcodes::HCMP:
                hcmpInsn();
                break;

            case Opcodes::RCMP:
                rcmpInsn();
                break;

            case Opcodes::BPUSH:
                bpushInsn();
                break;

            case Opcodes::SPUSH:
                spushInsn();
                break;

            case Opcodes::IPUSH:
                ipushInsn();
                break;

            case Opcodes::LPUSH:
                lpushInsn();
                break;

            case Opcodes::I2B:
                i2bInsn();
                break;

            case Opcodes::I2S:
                i2sInsn();
                break;

            case Opcodes::I2L:
                i2lInsn();
                break;

            case Opcodes::L2I:
                l2iInsn();
                break;

            case Opcodes::CONST_M1:
                constInsn(-1);
                break;

            case Opcodes::CONST_0:
                constInsn(0);
                break;

            case Opcodes::CONST_1:
                constInsn(1);
                break;

            case Opcodes::LCONST_0:
                lconstInsn(0);
                break;

            case Opcodes::LCONST_1:
                lconstInsn(1);
                break;

            case Opcodes::HCONST_NULL:
                hconst_nullInsn();
                break;

            case Opcodes::RCONST_NULL:
                rconst_nullInsn();

            case Opcodes::CALL:
                callInsn(wide);
                break;

            case Opcodes::RETURN:
                returnInsn();
                break;

            case Opcodes::IRETURN:
                ireturnInsn();
                break;

            case Opcodes::LRETURN:
                lreturnInsn();
                break;

            case Opcodes::HRETURN:
                hreturnInsn();
                break;

            case Opcodes::RRETURN:
                rreturnInsn();
                break;

            case Opcodes::WIDE:
                executeInstruction(true);
                break;

            case Opcodes::BREAKPOINT:
            case Opcodes::RESERVE1:
            case Opcodes::RESERVE2:
                break; // these are official instructions that are meant for debuggers or other special vm stuff

			default:
				std::cout << "Unknown instruction: " << static_cast<i32>(instruction) << ". TODO: proper error stuff\n";
				std::exit(1);
		}
	}

	void Executor::enterFunction(Function* function) {
		mFrame = mStack.enterFrame(function->getLocalCount(), function->getStackSize(), function, mCode, mPC);
        mPC = 0;
		mCode = function->getEntry();
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
		u64 value = ReadU8(&mCode[mPC], 0);
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
            std::cout << "error: field '" << field->getType().getClassName() << " " << field->getName() << "' is not a part of the class" <<
            object->getClass()->getModule()->getName() << ":" << object->getClass()->getName() << "\n";
            std::exit(1);
        }

        object->nullCheck();

        switch (field->getType().getInternalType()) {
            case Type::REFERENCE:
                mFrame->pushObject(object->getObject(field));
                break;
            case Type::HANDLE:
                mFrame->pushHandle(object->getHandle(field));
                break;
            case Type::BYTE:
                mFrame->push(object->getByte(field));
                break;
            case Type::SHORT:
                mFrame->push(object->getShort(field));
                break;
            case Type::INT:
                mFrame->push(object->getInt(field));
                break;
            case Type::LONG:
                mFrame->pushLong(object->getLong(field));
                break;
            case Type::CHAR:
                mFrame->push(object->getChar(field));
                break;
            case Type::FLOAT:
                std::cout << "float is NOT supported yet\n";
                std::exit(1);
            case Type::DOUBLE:
                std::cout << "double is NOT supported yet\n";
                std::exit(1);
            case Type::BOOL:
                mFrame->push(object->getBool(field));
                break;
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
                    std::cout << "error: field '" << field->getType().getClassName() << " " << field->getName() << "' is not a part of the class" <<
                              object->getClass()->getModule()->getName() << ":" << object->getClass()->getName() << "\n";
                    std::exit(1);
                }

                object->setObject(field, value);
                break;
            }
            case Type::HANDLE: {
                Handle value = mFrame->popHandle();
                ObjectRef object = mFrame->popObject();

                object->nullCheck();

                if (!object->isInstance(field->getClass())) {
                    std::cout << "error: field '" << field->getType().getClassName() << " " << field->getName() << "' is not a part of the class" <<
                              object->getClass()->getModule()->getName() << ":" << object->getClass()->getName() << "\n";
                    std::exit(1);
                }

                object->setHandle(field, value);
                break;
            }
            case Type::BYTE: {
                auto value = static_cast<Byte>(mFrame->pop());
                ObjectRef object = mFrame->popObject();

                object->nullCheck();

                if (!object->isInstance(field->getClass())) {
                    std::cout << "error: field '" << field->getType().getClassName() << " " << field->getName() << "' is not a part of the class" <<
                              object->getClass()->getModule()->getName() << ":" << object->getClass()->getName() << "\n";
                    std::exit(1);
                }

                object->setByte(field, value);
                break;
            }
            case Type::SHORT: {
                auto value = static_cast<Short>(mFrame->pop());
                ObjectRef object = mFrame->popObject();

                object->nullCheck();

                if (!object->isInstance(field->getClass())) {
                    std::cout << "error: field '" << field->getType().getClassName() << " " << field->getName() << "' is not a part of the class" <<
                              object->getClass()->getModule()->getName() << ":" << object->getClass()->getName() << "\n";
                    std::exit(1);
                }

                object->setShort(field, value);
                break;
            }
            case Type::INT: {
                Int value = mFrame->pop();
                ObjectRef object = mFrame->popObject();

                object->nullCheck();

                if (!object->isInstance(field->getClass())) {
                    std::cout << "error: field '" << field->getType().getClassName() << " " << field->getName() << "' is not a part of the class" <<
                              object->getClass()->getModule()->getName() << ":" << object->getClass()->getName() << "\n";
                    std::exit(1);
                }

                object->setInt(field, value);
                break;
            }
            case Type::LONG: {
                Long value = mFrame->popLong();
                ObjectRef object = mFrame->popObject();

                object->nullCheck();

                if (!object->isInstance(field->getClass())) {
                    std::cout << "error: field '" << field->getType().getClassName() << " " << field->getName() << "' is not a part of the class" <<
                              object->getClass()->getModule()->getName() << ":" << object->getClass()->getName() << "\n";
                    std::exit(1);
                }

                object->setLong(field, value);
                break;
            }
            case Type::CHAR: {
                auto value = static_cast<Char>(mFrame->pop());
                ObjectRef object = mFrame->popObject();

                object->nullCheck();

                if (!object->isInstance(field->getClass())) {
                    std::cout << "error: field '" << field->getType().getClassName() << " " << field->getName() << "' is not a part of the class" <<
                              object->getClass()->getModule()->getName() << ":" << object->getClass()->getName() << "\n";
                    std::exit(1);
                }

                object->setChar(field, value);
                break;
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
                    std::cout << "error: field '" << field->getType().getClassName() << " " << field->getName() << "' is not a part of the class" <<
                              object->getClass()->getModule()->getName() << ":" << object->getClass()->getName() << "\n";
                    std::exit(1);
                }

                object->setBool(field, value);
                break;
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
            Threading::LaunchThread(func);
            return;
        }

        if (func->isNative()) {
            if (func->getEntry() == nullptr) {
                Linker::LinkNativeFunction(func);
            }

            std::unique_ptr<JValue[]> nativeArgs = std::make_unique<JValue[]>(func->getArgumentTypes().size());

            u64 i = 0;
            for (auto& paramType : func->getArgumentTypes()) {
                if (paramType.is64Bit()) {
                    i64 value = mFrame->popLong();
                    nativeArgs[i++].L = value;
                } else {
                    i32 value = mFrame->pop();
                    nativeArgs[i++].I = value;
                }
            }

            if (func->getReturnType().getInternalType() == Type::VOID) {
                auto ptr = reinterpret_cast<NativeFunctionPtr<void>>(func->getEntry());
                ptr(mVM.getContext(), nativeArgs.get());

                return;
            } else {
                switch (func->getReturnType().getByteSize()) {
                    case 1: {
                        auto ptr = reinterpret_cast<NativeFunctionPtr<Byte>>(func->getEntry());
                        auto value = ptr(mVM.getContext(), nativeArgs.get());
                        mFrame->push(value);
                        break;
                    }
                    case 2: {
                        auto ptr = reinterpret_cast<NativeFunctionPtr<Short>>(func->getEntry());
                        auto value = ptr(mVM.getContext(), nativeArgs.get());
                        mFrame->push(value);
                        break;
                    }
                    case 4: {
                        auto ptr = reinterpret_cast<NativeFunctionPtr<Int>>(func->getEntry());
                        auto value = ptr(mVM.getContext(), nativeArgs.get());
                        mFrame->push(value);
                        break;
                    }
                    case 8: {
                        auto ptr = reinterpret_cast<NativeFunctionPtr<Long>>(func->getEntry());
                        auto value = ptr(mVM.getContext(), nativeArgs.get());
                        mFrame->pushLong(value);
                        break;
                    }
                    default:
                        std::cout << "native call error\n";
                        std::exit(1);
                }

                return;
            }
        }

        Stack::Frame* oldFrame = mFrame;
        enterFunction(func);

        u16 i = func->getNeededLocalsForArgs();

        for (auto it = func->getArgumentTypes().rbegin(); it != func->getArgumentTypes().rend(); ++it) {
            auto& arg = *it;

            switch (arg.getInternalType()) {
                case Type::REFERENCE: {
                    ObjectRef obj = oldFrame->popObject();

                    i -= 2;
                    mFrame->setLocalObject(i, obj);

                    break;
                }

                case Type::HANDLE: {
                    auto value = oldFrame->popHandle();

                    i -= 2;
                    mFrame->setLocalHandle(i, value);

                    break;
                }

                case Type::BYTE: {
                    auto value = oldFrame->pop();

                    i -= 1;
                    mFrame->setLocalInt(i, static_cast<Byte>(value));

                    break;
                }

                case Type::SHORT: {
                    auto value = oldFrame->pop();

                    i -= 1;
                    mFrame->setLocalInt(i, static_cast<Short>(value));

                    break;
                }

                case Type::INT: {
                    auto value = oldFrame->pop();

                    i -= 1;
                    mFrame->setLocalInt(i, static_cast<Int>(value));

                    break;
                }

                case Type::LONG: {
                    auto value = oldFrame->popLong();

                    i -= 2;
                    mFrame->setLocalLong(i, static_cast<Long>(value));

                    break;
                }

                case Type::CHAR: {
                    auto value = oldFrame->pop();

                    i -= 1;
                    mFrame->setLocalInt(i, static_cast<Char>(value));

                    break;
                }

                case Type::FLOAT:
                case Type::DOUBLE:
                    std::exit(3);
                case Type::BOOL: {
                    auto value = oldFrame->pop();

                    i -= 1;
                    mFrame->setLocalInt(i, value != 0);

                    break;
                }
            }
        }
    }

	void Executor::returnInsn() {
        mPC = mFrame->getReturnPC();
		mCode = mFrame->getReturnCode();
		mFrame = mStack.leaveFrame();

		if (mFrame == nullptr) {
			mReturned = true;
		}
	}

    void Executor::ireturnInsn() {
        i32 value = mFrame->pop();

        mPC = mFrame->getReturnPC();
        mCode = mFrame->getReturnCode();
        mFrame = mStack.leaveFrame();

        if (mFrame == nullptr) {
            mReturned = true;
            mReturnValue.I = value;
        } else {
            mFrame->push(value);
        }
    }

    void Executor::lreturnInsn() {
        i64 value = mFrame->popLong();

        mPC = mFrame->getReturnPC();
        mCode = mFrame->getReturnCode();
        mFrame = mStack.leaveFrame();

        if (mFrame == nullptr) {
            mReturned = true;
            mReturnValue.L = value;
        } else {
            mFrame->pushLong(value);
        }
    }

    void Executor::hreturnInsn() {
        Handle value = mFrame->popHandle();

        mPC = mFrame->getReturnPC();
        mCode = mFrame->getReturnCode();
        mFrame = mStack.leaveFrame();

        if (mFrame == nullptr) {
            mReturned = true;
            mReturnValue.H = value;
        } else {
            mFrame->pushHandle(value);
        }
    }

    void Executor::rreturnInsn() {
        ObjectRef value = mFrame->popObject();

        mPC = mFrame->getReturnPC();
        mCode = mFrame->getReturnCode();
        mFrame = mStack.leaveFrame();

        if (mFrame == nullptr) {
            mReturned = true;
            mReturnValue.R = value;
            std::cout << "warning: potential memory leak. native interface with objects is not stable yet!\n";
            value->addReference(); // the native value gets a reference
        } else {
            mFrame->pushObject(value);
        }
    }
}