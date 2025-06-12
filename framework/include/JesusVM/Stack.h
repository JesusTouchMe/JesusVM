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
            Frame(u16 localCount, u16 maxFrameSize, ConstPool& constPool, Module* module, Function* function, u8* returnCode, u32 returnPc);
            ~Frame();

            void push(i32 value);
            void pushLong(i64 value);
            void pushHandle(Handle handle);
            void pushObject(Object* object);

            i32 pop();
            i64 popLong();
            Handle popHandle();
            ObjectRef popObject();
            Object* popObjectWeak();

            void popValue();

            Object* extractThis(Method* method);

            void dup();
            void dup2();

            void swap();
            void swap2();

            Int getLocalInt(u16 index);
            Long getLocalLong(u16 index);
            Handle getLocalHandle(u16 index);
            ObjectRef getLocalObject(u16 index);
            Object* getLocalObjectWeak(u16 index);

            void setLocalInt(u16 index, Int value);
            void setLocalLong(u16 index, Long value);
            void setLocalHandle(u16 index, Handle value);
            void setLocalObject(u16 index, Object* object);

            void incLocalInt(u16 index, i16 increment);
            void incLocalLong(u16 index, i16 increment);

            ConstPool& getConstPool() const;
            Module* getCurrentModule() const;
            Function* getCurrentFunction() const;
            u8* getReturnCode() const;
            u32 getReturnPC() const;

        private:
            enum class ElementType : u8 {
                INT,
                LONG,
                HANDLE,
                REFERENCE,
            };

            std::unique_ptr<Frame> mPrevious;

            u16 mLocalCount;
            u16 mStackSize;
            u16 mStackTop;

            std::unique_ptr<i32[]> mLocals;
            std::unique_ptr<u8[]> mLocalTypes;
            std::unique_ptr<i32[]> mStack;
            std::vector<ElementType> mStackTypes;

            ConstPool& mConstPool;

            Module* mCurrentModule;
            Function* mCurrentFunction;

            u8* mReturnCode;
            u32 mReturnPC;

            void push1(i32 value);
            void push2(i64 value);

            i32 pop1();
            i64 pop2();

            void pushType(ElementType type);
            ElementType popType();

            i32 getLocal1(u16 index);
            i64 getLocal2(u16 index);

            void setLocal1(u16 index, i32 value);
            void setLocal2(u16 index, i64 value);

            moduleweb_stackmap_type_id getLocalType(u16 index);
            void setLocalType(u16 index, moduleweb_stackmap_type_id type);

            u32 getTypeSize(ElementType type);
        };

    public:
        Stack();

        Frame* getTopFrame() const;
        Frame* enterFrame(u16 localCount, u16 maxFrameSize, Function* function, u8* returnCode, u32 returnPc);
        Frame* leaveFrame();

    private:
        std::unique_ptr<Frame> mTop;
    };
}

#endif // JESUSVM_STACK_H
