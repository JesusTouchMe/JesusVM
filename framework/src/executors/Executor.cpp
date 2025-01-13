#include "JesusVM/bytecode/Opcodes.h"

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

	Executor::Executor(VThread& thread, Stack<0>& stack)
		: mThread(thread)
		, mStack(stack)
		, mFrame(stack.getTopFrame())
		, mPC(nullptr) {}

	void Executor::executeInstruction() {
		u8 instruction = *mPC++;

		switch (instruction) {
			case Opcodes::ADD:
				addInsn();
				break;

			case Opcodes::SUB:
				subInsn();
				break;

			default:
				std::cout << "Unknown instruction: " << instruction << ". TODO: proper error stuff\n";
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

	void Executor::returnInsn() {
		mPC = mFrame->getReturnAddress();
		mFrame = mStack.leaveFrame();

		if (mFrame == nullptr) {
			mThread.mIsActive = false;
		}
	}
}