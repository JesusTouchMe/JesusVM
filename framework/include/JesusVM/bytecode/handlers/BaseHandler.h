// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_BYTECODE_HANDLERS_BASEHANDLER_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_BYTECODE_HANDLERS_BASEHANDLER_H 1

#include "JesusVM/bytecode/Executor.h"

namespace JesusVM::BaseOpHandler {
    void AddHandlers(Executor::DispatchTable& table);

    namespace ops {
        void Add(Executor& executor);
        void Sub(Executor& executor);
        void Mul(Executor& executor);
        void Div(Executor& executor);
        void Rem(Executor& executor);

        void LAdd(Executor& executor);
        void LSub(Executor& executor);
        void LMul(Executor& executor);
        void LDiv(Executor& executor);
        void LRem(Executor& executor);

        void And(Executor& executor);
        void Or(Executor& executor);
        void Xor(Executor& executor);
        void Shl(Executor& executor);
        void Shr(Executor& executor);

        void LAnd(Executor& executor);
        void LOr(Executor& executor);
        void LXor(Executor& executor);
        void LShl(Executor& executor);
        void LShr(Executor& executor);

        void Not(Executor& executor);
        void Neg(Executor& executor);

        void LNot(Executor& executor);
        void LNeg(Executor& executor);

        void Pop(Executor& executor);
        void Pop2(Executor& executor);

        void Dup(Executor& executor);
        void Dup2(Executor& executor);

        void Swap(Executor& executor);
        void Swap2(Executor& executor);

        void Inc(Executor& executor);
        void LInc(Executor& executor);

        void ILoad(Executor& executor);
        void IStore(Executor& executor);

        void LLoad(Executor& executor);
        void LStore(Executor& executor);

        void HLoad(Executor& executor);
        void HStore(Executor& executor);

        void RLoad(Executor& executor);
        void RStore(Executor& executor);
        void RLoad0(Executor& executor);

        void BALoad(Executor& executor);
        void BAStore(Executor& executor);

        void CALoad(Executor& executor);
        void CAStore(Executor& executor);

        void SALoad(Executor& executor);
        void SAStore(Executor& executor);

        void IALoad(Executor& executor);
        void IAStore(Executor& executor);

        void LALoad(Executor& executor);
        void LAStore(Executor& executor);

        void HALoad(Executor& executor);
        void HAStore(Executor& executor);

        void RALoad(Executor& executor);
        void RAStore(Executor& executor);

        void ArrayLength(Executor& executor);

        void New(Executor& executor);
        void NewArray(Executor& executor);
        void RNewArray(Executor& executor);

        void IsInstance(Executor& executor);

        void GetField(Executor& executor);
        void SetField(Executor& executor);

        void GetGlobal(Executor& executor);
        void SetGlobal(Executor& executor);

        void JmpICmpEq(Executor& executor);
        void JmpICmpNe(Executor& executor);
        void JmpICmpLt(Executor& executor);
        void JmpICmpGt(Executor& executor);
        void JmpICmpLe(Executor& executor);
        void JmpICmpGe(Executor& executor);

        void JmpHCmpEq(Executor& executor);
        void JmpHCmpNe(Executor& executor);

        void JmpRCmpEq(Executor& executor);
        void JmpRCmpNe(Executor& executor);

        void JmpHNull(Executor& executor);
        void JmpHNonNull(Executor& executor);

        void JmpRNull(Executor& executor);
        void JmpRNonNull(Executor& executor);

        void JmpEq(Executor& executor);
        void JmpNe(Executor& executor);
        void JmpLt(Executor& executor);
        void JmpGt(Executor& executor);
        void JmpLe(Executor& executor);
        void JmpGe(Executor& executor);
        void Jmp(Executor& executor);

        void ICmp(Executor& executor);
        void LCmp(Executor& executor);
        void HCmp(Executor& executor);
        void RCmp(Executor& executor);

        void BPush(Executor& executor);
        void SPush(Executor& executor);
        void IPush(Executor& executor);
        void LPush(Executor& executor);

        void I2B(Executor& executor);
        void I2S(Executor& executor);
        void I2L(Executor& executor);
        void L2I(Executor& executor);

        void ConstM1(Executor& executor);
        void Const0(Executor& executor);
        void Const1(Executor& executor);

        void LConst0(Executor& executor);
        void LConst1(Executor& executor);

        void HConstNull(Executor& executor);

        void RConstNull(Executor& executor);

        void Call(Executor& executor);
        void CallVirtual(Executor& executor);

        void Return(Executor& executor);
        void IReturn(Executor& executor);
        void LReturn(Executor& executor);
        void HReturn(Executor& executor);
        void RReturn(Executor& executor);

        void Ldc(Executor& executor);

        void Wide(Executor& executor);
    }
}

#endif // JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_BYTECODE_HANDLERS_BASEHANDLER_H
