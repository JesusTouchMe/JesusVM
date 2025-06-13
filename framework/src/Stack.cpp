#include "JesusVM/Module.h"
#include "JesusVM/Stack.h"

#include "JesusVM/constpool/ConstPool.h"

#include <cassert>

namespace JesusVM {
    Stack::Frame::Frame(u16 stackSize, u16 localCount, ConstPool& constPool, Module* module, Function* function, u8* returnCode, u32 returnPc)
        : mPrevious(nullptr)
        , mStackSize(stackSize)
        , mStackTop(0)
        , mLocalCount(localCount)
        , mConstPool(constPool)
        , mCurrentModule(module)
        , mCurrentFunction(function)
        , mReturnCode(returnCode)
        , mReturnPC(returnPc) {
        constexpr size_t alignment = alignof(i64);
        size_t stackBytes      = stackSize * sizeof(i64);
        size_t stackTypesBytes = stackSize * sizeof(u8);
        size_t localsBytes     = localCount * sizeof(i64);
        size_t localTypesBytes = localCount * sizeof(u8);
        size_t size = stackBytes + stackTypesBytes + localsBytes + localTypesBytes + 3 * alignment;

        void* memory = std::malloc(size);

        auto base = reinterpret_cast<uintptr_t>(memory);

        auto align_up = [](uintptr_t& p, size_t a) {
            p = (p + a - 1) & ~(a - 1);
        };

        align_up(base, alignof(i64));
        mStack = reinterpret_cast<i64*>(base);
        base += stackBytes;

        align_up(base, alignof(u8));
        mStackTypes = reinterpret_cast<u8*>(base);
        base += stackTypesBytes;

        align_up(base, alignof(i64));
        mLocals = reinterpret_cast<i64*>(base);
        base += localsBytes;

        align_up(base, alignof(u8));
        mLocalTypes = reinterpret_cast<u8*>(base);
        base += localTypesBytes;

        assert(reinterpret_cast<uintptr_t>(mStack) == reinterpret_cast<uintptr_t>(memory));
    }

    Stack::Frame::~Frame() {
        for (u16 i = 0; i < mStackSize; i++) {
            if (mStackTypes[i] == STACKMAP_TYPE_REFERENCE) {
                auto object = reinterpret_cast<Object*>(mStack[i]);
                if (object != nullptr) object->removeReference();
            }
        }

        for (u16 i = 0; i < mLocalCount; i++) {
            if (mLocalTypes[i] == STACKMAP_TYPE_REFERENCE) {
                auto object = getLocalObjectWeak(i);
                if (object != nullptr) object->removeReference();
            }
        }

        std::free(mStack);
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

    u8* Stack::Frame::getReturnCode() const {
        return mReturnCode;
    }

    u32 Stack::Frame::getReturnPC() const {
        return mReturnPC;
    }

    void Stack::Frame::push(Long value) {
        if (mStackTop >= mStackSize)
            ThrowStackOverflow();

        mStack[mStackTop] = value;
        mStackTypes[mStackTop++] = STACKMAP_TYPE_LONG;
    }

    void Stack::Frame::pushHandle(Handle handle) {
        if (mStackTop >= mStackSize)
            ThrowStackOverflow();

        mStack[mStackTop] = reinterpret_cast<i64>(handle);
        mStackTypes[mStackTop++] = STACKMAP_TYPE_HANDLE;
    }

    void Stack::Frame::pushObject(Object* object) {
        if (mStackTop >= mStackSize)
            ThrowStackOverflow();

        mStack[mStackTop] = reinterpret_cast<i64>(object);
        mStackTypes[mStackTop++] = STACKMAP_TYPE_REFERENCE;

        if (object != nullptr) object->addReference();
    }

    Long Stack::Frame::pop() {
        if (mStackTop < 1)
            ThrowStackUnderflow();

        if (mStackTypes[--mStackTop] != STACKMAP_TYPE_LONG) {
            ThrowTypeMismatch(STACKMAP_TYPE_LONG, mStackTypes[mStackTop]);
        }
        return mStack[mStackTop];
    }

    Handle Stack::Frame::popHandle() {
        if (mStackTop < 1)
            ThrowStackUnderflow();

        if (mStackTypes[--mStackTop] != STACKMAP_TYPE_HANDLE) {
            ThrowTypeMismatch(STACKMAP_TYPE_HANDLE, mStackTypes[mStackTop]);
        }
        return reinterpret_cast<Handle>(mStack[mStackTop]);
    }

    ObjectRef Stack::Frame::popObject() {
        if (mStackTop < 1)
            ThrowStackUnderflow();

        if (mStackTypes[--mStackTop] != STACKMAP_TYPE_REFERENCE) {
            ThrowTypeMismatch(STACKMAP_TYPE_REFERENCE, mStackTypes[mStackTop]);
        }
        return { reinterpret_cast<Object*>(mStack[mStackTop]), false };
    }

    Object* Stack::Frame::popObjectWeak() {
        if (mStackTop < 1)
            ThrowStackUnderflow();

        if (mStackTypes[--mStackTop] != STACKMAP_TYPE_REFERENCE) {
            ThrowTypeMismatch(STACKMAP_TYPE_REFERENCE, mStackTypes[mStackTop]);
        }

        auto* object = reinterpret_cast<Object*>(mStack[mStackTop]);
        if (object != nullptr) object->removeReference();

        return object;
    }

    void Stack::Frame::popGeneric() {
        if (mStackTop < 1)
            ThrowStackUnderflow();

        if (mStackTypes[--mStackTop] == STACKMAP_TYPE_REFERENCE) {
            auto* object = reinterpret_cast<Object*>(mStack[mStackTop]);
            if (object != nullptr) object->removeReference();
        }
    }

    Object* Stack::Frame::extractThis(Method* method) {
        u32 index = mStackTop - method->getFunction()->getNeededLocalsForArgs();
        return reinterpret_cast<Object*>(mStack[index]);
    }

    void Stack::Frame::dup() {
        if (mStackTop < 1)
            ThrowStackUnderflow();
        if (mStackTop >= mStackSize)
            ThrowStackOverflow();

        i64 value = mStack[mStackTop - 1];
        u8 type = mStackTypes[mStackTop - 1];

        if (type == STACKMAP_TYPE_REFERENCE)
            reinterpret_cast<Object*>(value)->addReference();

        mStack[mStackTop] = value;
        mStackTypes[mStackTop] = type;
        mStackTop++;
    }

    void Stack::Frame::dupx1() {
        if (mStackTop < 2)
            ThrowStackUnderflow();
        if (mStackTop + 1 >= mStackSize)
            ThrowStackOverflow();

        i64 topValue = mStack[mStackTop - 1];
        i64 belowValue = mStack[mStackTop - 2];
        u8 topType = mStack[mStackTop - 1];
        u8 belowType = mStack[mStackTop - 2];

        if (topType == STACKMAP_TYPE_REFERENCE)
            reinterpret_cast<Object*>(topValue)->addReference();

        mStack[mStackTop] = belowValue;
        mStackTypes[mStackTop] = belowType;

        mStack[mStackTop - 2] = topValue;
        mStackTypes[mStackTop - 2] = topType;
        mStack[mStackTop - 1] = belowType;
        mStackTypes[mStackTop - 1] = belowType;

        mStackTop++;
    }

    void Stack::Frame::swap() {
        if (mStackTop < 2)
            ThrowStackUnderflow();

        std::swap(mStack[mStackTop - 1], mStack[mStackTop - 2]);
        std::swap(mStackTypes[mStackTop - 1], mStackTypes[mStackTop - 2]);
    }

    Long Stack::Frame::getLocal(u16 index) {
        if (index >= mLocalCount)
            ThrowOutOfBounds();

        if (mLocalTypes[index] != STACKMAP_TYPE_LONG)
            ThrowTypeMismatch(STACKMAP_TYPE_LONG, mLocalTypes[index]);

        return mLocals[index];
    }

    Handle Stack::Frame::getLocalHandle(u16 index) {
        if (index >= mLocalCount)
            ThrowOutOfBounds();

        if (mLocalTypes[index] != STACKMAP_TYPE_HANDLE)
            ThrowTypeMismatch(STACKMAP_TYPE_HANDLE, mLocalTypes[index]);

        return reinterpret_cast<Handle>(mLocals[index]);
    }

    ObjectRef Stack::Frame::getLocalObject(u16 index) {
        if (index >= mLocalCount)
            ThrowOutOfBounds();

        if (mLocalTypes[index] != STACKMAP_TYPE_REFERENCE)
            ThrowTypeMismatch(STACKMAP_TYPE_REFERENCE, mLocalTypes[index]);

        return reinterpret_cast<Object*>(mLocals[index]);
    }

    Object* Stack::Frame::getLocalObjectWeak(u16 index) {
        if (index >= mLocalCount)
            ThrowOutOfBounds();

        if (mLocalTypes[index] != STACKMAP_TYPE_REFERENCE)
            ThrowTypeMismatch(STACKMAP_TYPE_REFERENCE, mLocalTypes[index]);

        return reinterpret_cast<Object*>(mLocals[index]);
    }

    void Stack::Frame::setLocal(u16 index, Long value) {
        if (index >= mLocalCount)
            ThrowOutOfBounds();

        if (mLocalTypes[index] != STACKMAP_TYPE_LONG)
            ThrowTypeMismatch(STACKMAP_TYPE_LONG, mLocalTypes[index]);

        mLocals[index] = value;
    }

    void Stack::Frame::setLocalHandle(u16 index, Handle value) {
        if (index >= mLocalCount)
            ThrowOutOfBounds();

        if (mLocalTypes[index] != STACKMAP_TYPE_HANDLE)
            ThrowTypeMismatch(STACKMAP_TYPE_HANDLE, mLocalTypes[index]);

        mLocals[index] = reinterpret_cast<Long>(value);
    }

    void Stack::Frame::setLocalObject(u16 index, Object* object) {
        if (index >= mLocalCount)
            ThrowOutOfBounds();

        if (mLocalTypes[index] != STACKMAP_TYPE_REFERENCE)
            ThrowTypeMismatch(STACKMAP_TYPE_REFERENCE, mLocalTypes[index]);

        if (object != nullptr) object->addReference();

        mLocals[index] = reinterpret_cast<Long>(object);
    }

    void Stack::Frame::incLocal(u16 index, i16 increment) {
        if (index >= mLocalCount)
            ThrowOutOfBounds();

        if (mLocalTypes[index] != STACKMAP_TYPE_LONG)
            ThrowTypeMismatch(STACKMAP_TYPE_LONG, mLocalTypes[index]);

        mLocals[index] += increment;
    }

    void Stack::Frame::ThrowTypeMismatch(u8 expectedType, u8 receivedType) {
        std::cout << "Type mismatch: expected (" << expectedType << "), but received (" << receivedType << ")" << std::endl;
        std::exit(1);
    }

    void Stack::Frame::ThrowStackUnderflow() {
        std::cout << "Stack underflow\n";
        std::exit(1);
    }

    void Stack::Frame::ThrowStackOverflow() {
        std::cout << "Stack overflow\n";
        std::exit(1);
    }

    void Stack::Frame::ThrowOutOfBounds() {
        std::cout << "Out of bounds\n";
        std::exit(1);
    }

    Stack::Stack()
        : mTop(nullptr) {}

    Stack::Frame* Stack::getTopFrame() const {
        return mTop.get();
    }

    Stack::Frame* Stack::enterFrame(u16 stackSize, u16 localCount, Function* function, u8* returnCode, u32 returnPc) {
        auto frame = std::make_unique<Frame>(
                stackSize, localCount, function->getModule()->getConstPool(),
                function->getModule(), function, returnCode, returnPc);

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

        // TODO: this is not a permanent fix, just something to test epoch system
        // marked as todo so i can find this whenever i decide to go through the todo comments
        Threading::CurrentThread::GetMutationBuffer().advanceEpoch();

        return mTop.get();
    }
}