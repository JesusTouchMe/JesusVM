#include "JesusVM/Module.h"
#include "JesusVM/Stack.h"

#include "JesusVM/constpool/ConstPool.h"

namespace JesusVM {
    Stack::Frame::Frame(u16 localCount, u16 maxFrameSize, ConstPool& constPool, Module* module, Function* function, u8* returnAddress)
            : mPrevious(nullptr)
            , mLocalCount(localCount)
            , mStackSize(maxFrameSize)
            , mStackTop(0)
            , mLocals(std::make_unique<i32[]>(localCount))
            , mStack(std::make_unique<i32[]>(maxFrameSize))
            , mConstPool(constPool)
            , mCurrentModule(module)
            , mCurrentFunction(function)
            , mReturnAddress(returnAddress) {}

    void Stack::Frame::push(i32 value) {
        if (mStackTop >= mStackSize) {
            std::cout << "stackoverflow. todo: proper errors\n";
            std::exit(1);
        }
        mStack[mStackTop++] = value;
    }

    void Stack::Frame::pushLong(i64 value) {
        if (mStackTop + 2 > mStackSize) {
            std::cout << "stackoverflow (long). todo: proper errors\n";
            std::exit(1);
        }
        mStack[mStackTop++] = static_cast<i32>(value >> 32);
        mStack[mStackTop++] = static_cast<i32>(value & 0xFFFFFFFF);
    }

    i32 Stack::Frame::pop() {
        if (mStackTop <= 0) {
            std::cout << "stackunderflow. todo: proper errors\n";
            std::exit(1);
        }
        return mStack[--mStackTop];
    }

    i64 Stack::Frame::popLong() {
        if (mStackTop < 2) {
            std::cout << "stackunderflow (long). todo: proper errors\n";
            std::exit(1);
        }
        i32 low = mStack[--mStackTop];
        i32 high = mStack[--mStackTop];
        return (static_cast<i64>(high) << 32) | (static_cast<i64>(low) & 0xFFFFFFFF);
    }

    i8 Stack::Frame::getLocalByte(u16 index) {
        if (index >= mLocalCount) {
            std::cout << "too high local access. todo: proper errors\n";
            std::exit(1);
        }
        return static_cast<i8>(mLocals[index]);
    }

    i16 Stack::Frame::getLocalShort(u16 index) {
        if (index >= mLocalCount) {
            std::cout << "too high local access. todo: proper errors\n";
            std::exit(1);
        }
        return static_cast<i16>(mLocals[index]);
    }

    i32 Stack::Frame::getLocalInt(u16 index) {
        if (index >= mLocalCount) {
            std::cout << "too high local access. todo: proper errors\n";
            std::exit(1);
        }
        return static_cast<i32>(mLocals[index]);
    }

    i64 Stack::Frame::getLocalLong(u16 index) {
        if (index + 1 >= mLocalCount) {
            std::cout << "too high local access. todo: proper errors\n";
            std::exit(1);
        }
        i32 low = mLocals[index];
        i32 high = mLocals[index + 1];
        return (static_cast<i64>(high) << 32) | (static_cast<i64>(low) & 0xFFFFFFFF);
    }

    void Stack::Frame::setLocalByte(u16 index, i8 value) {
        if (index >= mLocalCount) {
            std::cout << "too high local access. todo: proper errors\n";
            std::exit(1);
        }
        mLocals[index] = static_cast<i32>(value);
    }

    void Stack::Frame::setLocalShort(u16 index, i16 value) {
        if (index >= mLocalCount) {
            std::cout << "too high local access. todo: proper errors\n";
            std::exit(1);
        }
        mLocals[index] = static_cast<i32>(value);
    }

    void Stack::Frame::setLocalInt(u16 index, i32 value) {
        if (index >= mLocalCount) {
            std::cout << "too high local access. todo: proper errors\n";
            std::exit(1);
        }
        mLocals[index] = value;
    }

    void Stack::Frame::setLocalLong(u16 index, i64 value) {
        if (index + 1 >= mLocalCount) {
            std::cout << "too high local access for long. todo: proper errors\n";
            std::exit(1);
        }
        mLocals[index] = static_cast<i32>(value & 0xFFFFFFFF);
        mLocals[index + 1] = static_cast<i32>(value >> 32);
    }

    ConstPool& Stack::Frame::getConstPool() const {
        return mConstPool;
    }

    Module* Stack::Frame::getCurrentModule() const {
        return mCurrentModule;
    }

    Function* Stack::Frame::getCurrentFunction() const {
        return mCurrentFunction;
    }

    u8* Stack::Frame::getReturnAddress() const {
        return mReturnAddress;
    }

    Stack::Frame* Stack::getTopFrame() const {
        return mTop.get();
    }

    Stack::Frame* Stack::enterFrame(u16 localCount, u16 maxFrameSize, Function* function, u8* returnAddress) {
        auto frame = std::make_unique<Frame>(localCount, maxFrameSize, function->getModule()->getConstPool(), function->getModule(), function, returnAddress);
        Frame* result = frame.get();

        frame->mPrevious = std::move(mTop);
        mTop = std::move(frame);

        return result;
    }

    Stack::Frame* Stack::leaveFrame() {
        if (!mTop) {
            std::cout << "stackunderflow. todo: proper errors\n";
            std::exit(1);
        }

        auto newFrame = std::move(mTop->mPrevious);
        if (!newFrame) {
            return nullptr;
        }

        Frame* result = newFrame.get();
        mTop = std::move(newFrame);

        return result;
    }
}