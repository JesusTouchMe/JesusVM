 #include "JesusVM/JesusVM.h"

#include "JesusVM/bytecode/Executor.h"
#include "JesusVM/bytecode/Opcodes.h"

#include "JesusVM/bytecode/handlers/BaseHandler.h"

#include "JesusVM/concurrency/Future.h"
#include "JesusVM/concurrency/Scheduler.h"

#include <ranges>

#undef VOID
#undef BOOL

namespace JesusVM {
    Executor::DispatchTable Executor::mDispatchTable{};
    std::array<Executor::DispatchTable, 16> Executor::mExtDispatchTables{};

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
        , mCode(nullptr)
        , mPC(0)
        , mWide(false)
		, mReturnDepth(1)
        , mReturnValue(static_cast<Long>(0))
        , mAwaitingFuture(nullptr)
        , mPaused(false) {}

    Stack::Frame* Executor::getFrame() const {
        return mFrame;
    }

    Executor::WideGuard Executor::getWideGuard() {
        return WideGuard(*this);
    }

    JValue Executor::getReturnValue() const {
        return mReturnValue;
    }

    JValue& Executor::returnValue() {
        return mReturnValue;
    }

    void Executor::setWide(bool value) {
        mWide = value;
    }

    void Executor::branch(i16 branch) {
        mPC += branch;
    }

	void Executor::enterFunction(Function* function) {
		mFrame = mStack.enterFrame(function->getStackSize(), function->getLocalCount(), function, mCode, mPC);
        mPC = 0;
		mCode = function->getEntry();
        mReturnDepth++;
	}

    void Executor::leaveFunction() {
        mPC = mFrame->getReturnPC();
        mCode = mFrame->getReturnCode();
        mFrame = mStack.leaveFrame();

        mReturnDepth--;
    }

    void Executor::runUntilComplete() {
        while (!isComplete()) {
            step();
        }
    }

    bool Executor::runUntilYield() {
        while (!isComplete()) {
            step();

            if (isAwaiting()) return false;
        }

        return true;
    }

    void Executor::await(Future* future) {
        mAwaitingFuture = future;
        mPaused = true;

        Scheduler::PauseCurrentTaskOnFuture(future);
    }

    void Executor::resume() {
        mAwaitingFuture = nullptr;
        mPaused = false;
    }

    bool Executor::isAwaiting() const {
        return mPaused;
    }

    bool Executor::isComplete() const {
        return mReturnDepth > 0;
    }

    void Executor::step() {
		u8 opcode = fetch();
        if (opcode >= 0xF0 && opcode <= 0xFF) {
            DispatchTable& table = mExtDispatchTables[opcode - 0xF0];
            u8 extOpcode = fetch();
            OpHandler handler = table[extOpcode];

            if (handler == nullptr) {
                ThrowInvalidOpcode(extOpcode);
            }

            handler(*this);
        } else if (opcode == Opcodes::NOP) {
            return;
        } else {
            OpHandler handler = mDispatchTable[opcode];
            if (handler == nullptr) {
                ThrowInvalidOpcode(opcode);
            }

            handler(*this);
        }
	}

    u8 Executor::fetch() {
        if (mPC >= mFrame->getCurrentFunction()->getBytecodeSize()) {
            ThrowFetchOutOfBounds();
        }

        const u8 value = ReadU8(mCode, mPC);
        mPC += 1;

        return value;
    }

    u16 Executor::fetchShort() {
        if (mPC + 1 >= mFrame->getCurrentFunction()->getBytecodeSize()) {
            ThrowFetchOutOfBounds();
        }

        const u16 value = ReadU16(mCode, mPC);
        mPC += 2;

        return value;
    }

    u32 Executor::fetchInt() {
        if (mPC + 3 >= mFrame->getCurrentFunction()->getBytecodeSize()) {
            ThrowFetchOutOfBounds();
        }

        const u32 value = ReadU32(mCode, mPC);
        mPC += 4;

        return value;
    }

    u64 Executor::fetchLong() {
        if (mPC + 7 >= mFrame->getCurrentFunction()->getBytecodeSize()) {
            ThrowFetchOutOfBounds();
        }

        const u64 value = ReadU64(mCode, mPC);
        mPC += 8;

        return value;
    }

    void Executor::InitDispatch() {
        BaseOpHandler::AddHandlers(mDispatchTable);
    }

    void Executor::ThrowFetchOutOfBounds() {
        std::cout << "Attempted to fetch outside bounds of code section\n";
        std::exit(1);
    }

    void Executor::ThrowInvalidOpcode(u32 opcode) {
        std::cout << "Invalid opcode: 0x" << std::hex << opcode << "\n";
        std::exit(1);
    }
}