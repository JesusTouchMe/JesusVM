#ifndef JESUSVM_STACK_H
#define JESUSVM_STACK_H

#include "types.h"

#include "JesusVM/JesusNative.h"

#include "JesusVM/heap/Method.h"

#include "moduleweb/stackmap_attribute.h"

#include <concepts>
#include <iostream>
#include <memory>
#include <vector>

namespace JesusVM {
    class ConstPool;
    class Function;
    class Module;
    class Object;
    class ObjectRef;
    class NonNullObjectRef;

    class Stack {
    public:
        class Frame {
        friend class Stack;
        public:
            Frame(u16 stackSize, u16 localCount, ConstPool& constPool, Module* module, Function* function, u8* returnCode, u32 returnPc);
            ~Frame();

            ConstPool& getConstPool() const;
            Module* getCurrentModule() const;
            Function* getCurrentFunction() const;
            u8* getReturnCode() const;
            u32 getReturnPC() const;

            void push(Long value);
            void pushHandle(Handle handle);
            void pushObject(Object* object);

            Long pop();
            Handle popHandle();
            ObjectRef popObject();
            Object* popObjectWeak();

            void popGeneric();

            Object* extractThis(Method* method);

            void dup();
            void dup2();
            void dupx1();
            void dupx2();

            void swap();

            Long getLocal(u16 index);
            Handle getLocalHandle(u16 index);
            ObjectRef getLocalObject(u16 index);
            Object* getLocalObjectWeak(u16 index);

            void setLocal(u16 index, Long value);
            void setLocalHandle(u16 index, Handle value);
            void setLocalObject(u16 index, Object* object);

            void incLocal(u16 index, i16 increment);

        private:
            std::unique_ptr<Frame> mPrevious;

            u16 mStackSize;
            u16 mStackTop;
            u16 mLocalCount;

            i64* mStack;
            u8* mStackTypes;
            i64* mLocals;
            u8* mLocalTypes;

            ConstPool& mConstPool;

            Module* mCurrentModule;
            Function* mCurrentFunction;

            u8* mReturnCode;
            u32 mReturnPC;

            static void ThrowTypeMismatch(u8 expectedType, u8 receivedType);
            static void ThrowStackUnderflow();
            static void ThrowStackOverflow();
            static void ThrowOutOfBounds();
        };

    public:
        Stack();

        Frame* getTopFrame() const;
        Frame* enterFrame(u16 stackSize, u16 localCount, Function* function, u8* returnCode, u32 returnPc);
        Frame* leaveFrame();

    private:
        std::unique_ptr<Frame> mTop;
    };
}

#endif // JESUSVM_STACK_H
