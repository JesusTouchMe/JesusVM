#include "JesusVM/JesusVM.h"

#include "JesusVM/bytecode/Opcodes.h"

#include "JesusVM/constpool/ConstantFunc.h"
#include "JesusVM/constpool/ConstPool.h"

#include "JesusVM/executors/Executor.h"
#include "JesusVM/executors/VThread.h"

namespace JesusVM {
	constexpr u8 ReadU8(const u8* data, u32 index) {
		return data[index];
	}

	constexpr u16 ReadU16(const u8* data, size_t index) {
		return (static_cast<u16>(data[index]) << 0) |
			(static_cast<u16>(data[index + 1]) << 8);
	}

	constexpr u32 ReadU32(const u8* data, size_t index) {
		return (static_cast<u32>(data[index]) << 0) |
			(static_cast<u32>(data[index + 1]) << 8) |
			(static_cast<u32>(data[index + 2]) << 16) |
			(static_cast<u32>(data[index + 3]) << 24);
	}

	constexpr u64 ReadU64(const u8* data, size_t index) {
		return (static_cast<u64>(data[index]) << 0) |
			(static_cast<u64>(data[index + 1]) << 8) |
			(static_cast<u64>(data[index + 2]) << 16) |
			(static_cast<u64>(data[index + 3]) << 24) |
			(static_cast<u64>(data[index + 4]) << 32) |
			(static_cast<u64>(data[index + 5]) << 40) |
			(static_cast<u64>(data[index + 6]) << 48) |
			(static_cast<u64>(data[index + 7]) << 56);
	}

	Executor::Executor(JesusVM& vm, VThread& thread, Stack& stack)
        : mVM(vm)
		, mThread(thread)
		, mStack(stack)
		, mFrame(stack.getTopFrame())
		, mPC(nullptr) {}

	void Executor::executeInstruction(bool wide) {
        if (mPC >= mFrame->getCurrentFunction()->getEntry() + mFrame->getCurrentFunction()->getBytecodeSize()) {
            std::cout << "bad execution!!!\n";
            std::exit(1);
        }

		u8 instruction = *mPC++;

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

            case Opcodes::LAND:
                landInsn();
                break;

            case Opcodes::LOR:
                lorInsn();
                break;

            case Opcodes::LXOR:
                lxorInsn();
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

            case Opcodes::ICMP:
                icmpInsn();
                break;

            case Opcodes::LCMP:
                lcmpInsn();
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

            case Opcodes::WIDE:
                executeInstruction(true);
                break;

			default:
				std::cout << "Unknown instruction: " << static_cast<i32>(instruction) << ". TODO: proper error stuff\n";
				std::exit(1);
		}
	}

	void Executor::enterFunction(Function* function) {
		mFrame = mStack.enterFrame(function->getLocalCount(), function->getStackSize(), function, mPC);
		mPC = function->getEntry();
	}
	
	u8 Executor::getByte() {
		u8 value = ReadU8(mPC, 0);
		mPC += 1;

		return value;
	}
	
	u16 Executor::getShort() {
		u16 value = ReadU16(mPC, 0);
		mPC += 2;

		return value;
	}
	
	u32 Executor::getInt() {
		u32 value = ReadU32(mPC, 0);
		mPC += 4;

		return value;
	}
	
	u64 Executor::getLong() {
		u64 value = ReadU8(mPC, 0);
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

	void Executor::constInsn(i32 value) {
		mFrame->push(value);
	}
	
	void Executor::lconstInsn(i64 value) {
		mFrame->pushLong(value);
	}

    void Executor::callInsn(bool wide) {
        u32 index = wide ? getInt() : getShort();
        Constant* constant = mFrame->getConstPool().get(index);

        if (constant == nullptr) {
            std::cout << "constant index out of bounds. todo: better errors\n";
            std::exit(1);
        }

        if (constant->getType() != Constant::Type::FUNCTION) {
            std::cout << "calling non-function constant. todo: better errors\n";
            std::exit(1);
        }

        Function* func = reinterpret_cast<ConstantFunc*>(constant)->getFunction();

        if (func->getModifiers() & Function::NATIVE) {
            std::unique_ptr<JValue[]> nativeArgs = std::make_unique<JValue[]>(func->getType()->getArgumentTypes().size());

            u64 i = 0;
            for (auto& paramType : func->getType()->getArgumentTypes()) {
                if (paramType->isPrimitive64() || paramType->isReference()) {
                    i64 value = mFrame->popLong();
                    nativeArgs[i++].L = value;
                } else {
                    i32 value = mFrame->pop();
                    nativeArgs[i++].I = value;
                }
            }

            if (func->getType()->getReturnType()->isVoid()) {
                auto ptr = reinterpret_cast<NativeFunctionPtr<void>>(func->getEntry());
                ptr(mVM.getContext(), nativeArgs.get());

                return;
            } else {
                if (func->getType()->isPrimitive8()) {
                    auto ptr = reinterpret_cast<NativeFunctionPtr<Byte>>(func->getEntry());
                    auto value = ptr(mVM.getContext(), nativeArgs.get());

                    mFrame->push(value);
                } else if (func->getType()->isPrimitive16()) {
                    auto ptr = reinterpret_cast<NativeFunctionPtr<Short>>(func->getEntry());
                    auto value = ptr(mVM.getContext(), nativeArgs.get());

                    mFrame->push(value);
                } else if (func->getType()->isPrimitive32()) {
                    auto ptr = reinterpret_cast<NativeFunctionPtr<Int>>(func->getEntry());
                    auto value = ptr(mVM.getContext(), nativeArgs.get());

                    mFrame->push(value);
                } else if (func->getType()->isPrimitive64()) {
                    auto ptr = reinterpret_cast<NativeFunctionPtr<Long>>(func->getEntry());
                    auto value = ptr(mVM.getContext(), nativeArgs.get());

                    mFrame->pushLong(value);
                } else if (func->getType()->isReference()) {
                    auto ptr = reinterpret_cast<NativeFunctionPtr<JObject>>(func->getEntry());
                    auto value = ptr(mVM.getContext(), nativeArgs.get());

                    mFrame->pushLong(reinterpret_cast<i64>(value));
                } else {
                    std::cout << "native call error\n";
                    std::exit(1);
                }

                return;
            }
        }

        std::vector<i32> args;
        args.reserve(func->getType()->getArgumentTypes().size());

        for (auto& paramType : func->getType()->getArgumentTypes()) {
            if (paramType->isPrimitive64() || paramType->isReference()) {
                i64 value = mFrame->popLong();
                args.push_back(static_cast<i32>(value >> 32));
                args.push_back(static_cast<i32>(value & 0xFFFFFFFF));
            } else {
                args.push_back(mFrame->pop());
            }
        }

        enterFunction(func);

        u16 i = 0;
        for (auto arg : args) {
            mFrame->setLocalInt(i++, arg);
        }
    }

	void Executor::returnInsn() {
		mPC = mFrame->getReturnAddress();
		mFrame = mStack.leaveFrame();

		if (mFrame == nullptr) {
			mThread.mIsActive = false;
		}
	}

    void Executor::ireturnInsn() {
        i32 value = mFrame->pop();

        mPC = mFrame->getReturnAddress();
        mFrame = mStack.leaveFrame();

        if (mFrame == nullptr) {
            mThread.mIsActive = false;
        } else {
            mFrame->push(value);
        }
    }

    void Executor::lreturnInsn() {
        i64 value = mFrame->popLong();

        mPC = mFrame->getReturnAddress();
        mFrame = mStack.leaveFrame();

        if (mFrame == nullptr) {
            mThread.mIsActive = false;
        } else {
            mFrame->pushLong(value);
        }
    }
}