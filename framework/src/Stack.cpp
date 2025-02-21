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
            , mReturnAddress(returnAddress) {
        mTypes.reserve(maxFrameSize);
    }

    void Stack::Frame::push(i32 value) {
        push1(value);
        pushType(ElementType::INT);
    }

    void Stack::Frame::pushLong(i64 value) {
        push2(value);
        pushType(ElementType::LONG);
    }

    void Stack::Frame::pushHandle(Handle handle) {
#       ifdef JESUSVM_CONFIG_32BIT
        push1(reinterpret_cast<i32>(handle));
#       else
        push2(reinterpret_cast<i64>(handle));
#       endif

        pushType(ElementType::HANDLE);
    }

    void Stack::Frame::pushObject(Object* object) {
#       ifdef JESUSVM_CONFIG_32BIT
        push1(reinterpret_cast<i32>(object));
#       else
        push2(reinterpret_cast<i64>(object));
#       endif

        pushType(ElementType::REFERENCE);

        object->addReference();
    }

    i32 Stack::Frame::pop() {
        ElementType type = popType();
        if (type != ElementType::INT) {
            std::cout << "type mismatch: popping category 1 primitive type, but got a different type\n";
            std::exit(1);
        }

        return pop1();
    }

    i64 Stack::Frame::popLong() {
        ElementType type = popType();
        if (type != ElementType::LONG) {
            std::cout << "type mismatch: popping category 2 primitive type, but got a different type\n";
            std::exit(1);
        }

        return pop2();
    }

    Handle Stack::Frame::popHandle() {
        ElementType type = popType();
        if (type != ElementType::HANDLE) {
            std::cout << "type mismatch: popping handle type, but got a different type\n";
            std::exit(1);
        }

#       ifdef JESUSVM_CONFIG_32BIT
        return reinterpret_cast<Handle>(pop1());
#       else
        return reinterpret_cast<Handle>(pop2());
#       endif
    }

    ObjectRef Stack::Frame::popObject() {
        ElementType type = popType();
        if (type != ElementType::REFERENCE) {
            std::cout << "type mismatch: popping reference type, but got a different type\n";
            std::exit(1);
        }

        Object* obj;

#       ifdef JESUSVM_CONFIG_32BIT
        obj = reinterpret_cast<Object*>(pop1());
#       else
        obj = reinterpret_cast<Object*>(pop2());
#       endif

        return {obj, false}; // we don't increment or decrement the objects refcount here because we lose our reference and caller gets a reference
    }

    // Potentially dangerous!
    Object* Stack::Frame::popObjectWeak() {
        ElementType type = popType();
        if (type != ElementType::REFERENCE) {
            std::cout << "type mismatch: popping reference type, but got a different type\n";
            std::exit(1);
        }

        Object* obj;

#       ifdef JESUSVM_CONFIG_32BIT
        obj = reinterpret_cast<Object*>(pop1());
#       else
        obj = reinterpret_cast<Object*>(pop2());
#       endif

        obj->removeReference();

        return obj;
    }

    void Stack::Frame::dup() {
        if (mStackTop < 1) {
            std::cout << "stackunderflow. todo: proper errors\n";
            std::exit(1);
        }

        ElementType type = mTypes.back();

        i32 top = mStack[mStackTop - 1];

#       ifdef JESUSVM_CONFIG_32BIT
        if (type == ElementType::REFERENCE) {
            auto obj = reinterpret_cast<Object*>(top);
            obj->addReference();
        }
#       else
        if (type == ElementType::REFERENCE) {
            std::cout << "type mismatch: popping category 1 reference type in 64 bit mode";
            std::exit(1);
        }
#       endif

        push1(top);
        pushType(type);
    }

    void Stack::Frame::dup2() {
        if (mStackTop < 2) {
            std::cout << "stackunderflow (long). todo: proper errors\n";
            std::exit(1);
        }

        ElementType type = mTypes.back();

        if (type == ElementType::INT) {
            std::cout << "attempt to dup a category 1 type with a category 2 dup\n";
            std::exit(1);
        }

        i32 low = mStack[mStackTop - 1];
        i32 high = mStack[mStackTop - 2];
        i64 top = (static_cast<i64>(high) << 32) | (static_cast<i64>(low) & 0xFFFFFFFF);

#       ifdef JESUSVM_CONFIG_32BIT
        if (type == ElementType::REFERENCE) {
            std::cout << "type mismatch: popping category 2 reference type in 32 bit mode";
            std::exit(1);
        }
#       else
        if (type == ElementType::REFERENCE) {
            auto obj = reinterpret_cast<Object*>(top);
            obj->addReference();
        }
#       endif

        push2(top);
        pushType(type);
    }

    void Stack::Frame::swap() {
        if (mStackTop < 2) {
            std::cout << "swap: stackunderflow\n";
            std::exit(1);
        }

        ElementType t1 = popType();
        ElementType t2 = popType();

        if (getTypeSize(t1) > 1 || getTypeSize(t2) > 1) {
            std::cout << "swap: type mismatch: big types\n";
            std::exit(1);
        }

        std::swap(mStack[mStackTop - 1], mStack[mStackTop - 2]);

        pushType(t1);
        pushType(t2);
    }

    void Stack::Frame::swap2() {
        if (mStackTop < 4) {
            std::cout << "swap2: stackunderflow\n";
            std::exit(1);
        }

        ElementType t1 = popType();
        ElementType t2 = popType();

        if (getTypeSize(t1) == 2 && getTypeSize(t2) == 2) {
            std::swap(mStack[mStackTop - 1], mStack[mStackTop - 3]);
            std::swap(mStack[mStackTop - 2], mStack[mStackTop - 4]);

            pushType(t1);
            pushType(t2);
        } else {
            ElementType t3 = popType();
            ElementType t4 = popType();

            if (getTypeSize(t1) > 1 || getTypeSize(t2) > 1 ||
                getTypeSize(t3) > 1 || getTypeSize(t4) > 1) {
                std::cout << "swap2: type mismatch: expected 2 category 2 or 4 category 1 types\n";
                std::exit(1);
            }

            std::swap(mStack[mStackTop - 1], mStack[mStackTop - 3]);
            std::swap(mStack[mStackTop - 2], mStack[mStackTop - 4]);

            pushType(t1);
            pushType(t2);
            pushType(t3);
            pushType(t4);
        }
    }

    Int Stack::Frame::getLocalInt(u16 index) {
        if (index >= mLocalCount) {
            std::cout << "too high local access. todo: proper errors\n";
            std::exit(1);
        }
        return static_cast<i32>(mLocals[index]);
    }

    Long Stack::Frame::getLocalLong(u16 index) {
        if (index + 1 >= mLocalCount) {
            std::cout << "too high local access. todo: proper errors\n";
            std::exit(1);
        }
        i32 low = mLocals[index];
        i32 high = mLocals[index + 1];
        return (static_cast<i64>(high) << 32) | (static_cast<i64>(low) & 0xFFFFFFFF);
    }

    Handle Stack::Frame::getLocalHandle(u16 index) {
#       ifdef JESUSVM_CONFIG_32BIT
        return reinterpret_cast<Handle>(getLocalInt(index));
#       else
        return reinterpret_cast<Handle>(getLocalLong(index));
#       endif
    }

    ObjectRef Stack::Frame::getLocalObject(u16 index) {
        Object* obj;

#       ifdef JESUSVM_CONFIG_32BIT
        obj = reinterpret_cast<Object*>(getLocalInt(index));
#       else
        obj = reinterpret_cast<Object*>(getLocalLong(index));
#       endif

        return obj;
    }

    Object* Stack::Frame::getLocalObjectWeak(u16 index) {
        Object* obj;

#       ifdef JESUSVM_CONFIG_32BIT
        obj = reinterpret_cast<Object*>(getLocalInt(index));
#       else
        obj = reinterpret_cast<Object*>(getLocalLong(index));
#       endif

        return obj;
    }

    void Stack::Frame::setLocalInt(u16 index, Int value) {
        if (index >= mLocalCount) {
            std::cout << "too high local access. todo: proper errors\n";
            std::exit(1);
        }
        mLocals[index] = value;
    }

    void Stack::Frame::setLocalLong(u16 index, Long value) {
        if (index + 1 >= mLocalCount) {
            std::cout << "too high local access for long. todo: proper errors\n";
            std::exit(1);
        }
        mLocals[index] = static_cast<i32>(value & 0xFFFFFFFF);
        mLocals[index + 1] = static_cast<i32>(value >> 32);
    }

    void Stack::Frame::setLocalHandle(u16 index, Handle value) {
#       ifdef JESUSVM_CONFIG_32BIT
        setLocalInt(index, reinterpret_cast<i32>(value));
#       else
        setLocalLong(index, reinterpret_cast<i64>(value));
#       endif
    }

    void Stack::Frame::setLocalObject(u16 index, Object* object) {
#       ifdef JESUSVM_CONFIG_32BIT
        auto initial = reinterpret_cast<Object*>(getLocalInt(index));
        if (initial != nullptr)
            initial->removeReference();

        setLocalInt(index, reinterpret_cast<i32>(object));
#       else
        auto initial = reinterpret_cast<Object*>(getLocalLong(index));
        if (initial != nullptr)
            initial->removeReference();

        setLocalLong(index, reinterpret_cast<i64>(object));
#       endif

        object->addReference();
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

    void Stack::Frame::push1(i32 value) {
        if (mStackTop >= mStackSize) {
            std::cout << "stackoverflow. todo: proper errors\n";
            std::exit(1);
        }
        mStack[mStackTop++] = value;
    }

    void Stack::Frame::push2(i64 value) {
        if (mStackTop + 2 > mStackSize) {
            std::cout << "stackoverflow (long). todo: proper errors\n";
            std::exit(1);
        }
        mStack[mStackTop++] = static_cast<i32>(value >> 32);
        mStack[mStackTop++] = static_cast<i32>(value & 0xFFFFFFFF);
    }

    i32 Stack::Frame::pop1() {
        if (mStackTop <= 0) {
            std::cout << "stackunderflow. todo: proper errors\n";
            std::exit(1);
        }
        return mStack[--mStackTop];
    }

    i64 Stack::Frame::pop2() {
        if (mStackTop < 2) {
            std::cout << "stackunderflow (long). todo: proper errors\n";
            std::exit(1);
        }
        i32 low = mStack[--mStackTop];
        i32 high = mStack[--mStackTop];
        return (static_cast<i64>(high) << 32) | (static_cast<i64>(low) & 0xFFFFFFFF);
    }

    void Stack::Frame::pushType(Stack::Frame::ElementType type) {
        mTypes.push_back(type);
    }

    Stack::Frame::ElementType Stack::Frame::popType() {
        ElementType res = mTypes.back();
        mTypes.pop_back();
        return res;
    }

    u32 Stack::Frame::getTypeSize(Stack::Frame::ElementType type) {
        switch (type) {
#           ifdef JESUSVM_CONFIG_32BIT
            case ElementType::INT:
            case ElementType::HANDLE:
            case ElementType::REFERENCE:
                return 1;
            case ElementType::LONG:
                return 2;
#           else
            case ElementType::INT:
                return 1;
            case ElementType::HANDLE:
            case ElementType::REFERENCE:
            case ElementType::LONG:
                return 2;
#           endif
        }
    }

    Stack::Stack()
        : mTop(nullptr) {}

    Stack::Frame* Stack::getTopFrame() const {
        return mTop.get();
    }

    Stack::Frame* Stack::enterFrame(u16 localCount, u16 maxFrameSize, Function* function, u8* returnAddress) {
        auto frame = std::make_unique<Frame>(
                localCount, maxFrameSize, function->getModule()->getConstPool(),
                function->getModule(), function, returnAddress);

        frame->mPrevious = std::move(mTop);
        mTop = std::move(frame);

        return mTop.get();
    }

    Stack::Frame* Stack::leaveFrame() {
        if (mTop == nullptr) {
            std::cout << "stackunderflow. todo: proper errors\n";
            std::exit(1);
        }

        auto oldFrame = std::move(mTop);
        mTop = std::move(oldFrame->mPrevious);

        return mTop.get();
    }
}