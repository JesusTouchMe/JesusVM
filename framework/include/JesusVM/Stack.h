#ifndef JESUSVM_STACK_H
#define JESUSVM_STACK_H

#include "types.h"

#include "JesusVM/Module.h"

#include <concepts>
#include <iostream>

namespace JesusVM {
    class ConstPool;
    class Module;

    class Stack {
    public:
        class Frame {
            friend class Stack;
        public:
            Frame(u16 localCount, u16 maxFrameSize, ConstPool& constPool, Module* module, Function* function, u8* returnAddress);

            void push(i32 value);
            void pushLong(i64 value);
            i32 pop();
            i64 popLong();

            i8 getLocalByte(u16 index);
            i16 getLocalShort(u16 index);
            i32 getLocalInt(u16 index);
            i64 getLocalLong(u16 index);

            void setLocalByte(u16 index, i8 value);
            void setLocalShort(u16 index, i16 value);
            void setLocalInt(u16 index, i32 value);
            void setLocalLong(u16 index, i64 value);

            const ConstPool& getConstPool() const;
            Module* getCurrentModule() const;
            Function* getCurrentFunction() const;
            u8* getReturnAddress() const;

        private:
            std::unique_ptr<Frame> mPrevious;

            u16 mLocalCount;
            u16 mStackSize;
            u16 mStackTop;

            std::unique_ptr<i32[]> mLocals;
            std::unique_ptr<i32[]> mStack;

            ConstPool& mConstPool;

            Module* mCurrentModule;
            Function* mCurrentFunction;

            u8* mReturnAddress;
        };

    public:
        Frame* getTopFrame() const;
        Frame* enterFrame(u16 localCount, u16 maxFrameSize, Function* function, u8* returnAddress);
        Frame* leaveFrame();

    private:
        std::unique_ptr<Frame> mTop;
    };
}

#endif // JESUSVM_STACK_H
