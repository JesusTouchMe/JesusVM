#include "JesusVM/bytecode/Opcodes.h"

#include "JesusVM/executors/Executor.h"

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

	Executor::Executor(Stack<0>& stack)
		: mStack(stack)
		, mFrame(stack.getTopFrame()) {}

	void Executor::executeInstruction() {
		u8 instruction = *mPC++;

		switch (instruction) {
			case Opcodes::ADD:
				addInsn();
				break;

			case Opcodes::SUB:
				subInsn();
				break;

			case Opcodes::IPUSH_8:
				ipush8Insn();
				break;

			case Opcodes::PRINT:
				printInsn();
				break;

			case Opcodes::EXIT:
				std::exit(0);

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

	void Executor::ipush8Insn() {
		i8 value = getByte();
		mFrame->push(value);
	}

	void Executor::printInsn() {
		i32 value = mFrame->pop();
		std::cout << value << "\n";
	}
}