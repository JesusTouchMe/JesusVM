// Copyright 2025 JesusTouchMe

#include "JesusVM/bytecode/handlers/BaseHandler.h"


#include "JesusVM/bytecode/Opcodes.h"

#include "JesusVM/constpool/ConstantClass.h"
#include "JesusVM/constpool/ConstantField.h"
#include "JesusVM/constpool/ConstantFunc.h"
#include "JesusVM/constpool/ConstantGlobalVar.h"
#include "JesusVM/constpool/ConstantMethod.h"
#include "JesusVM/constpool/ConstPool.h"

#include "JesusVM/heap/Object.h"

#include "JesusVM/JesusVM.h"
#include "JesusVM/Module.h"

namespace JesusVM::BaseOpHandler {
    void AddHandlers(Executor::DispatchTable& table) {
        table[Opcodes::ADD] = ops::Add;
        table[Opcodes::SUB] = ops::Sub;
        table[Opcodes::MUL] = ops::Mul;
        table[Opcodes::DIV] = ops::Div;
        table[Opcodes::REM] = ops::Rem;
        table[Opcodes::AND] = ops::And;
        table[Opcodes::OR] = ops::Or;
        table[Opcodes::XOR] = ops::Xor;
        table[Opcodes::SHL] = ops::Shl;
        table[Opcodes::SHR] = ops::Shr;
        table[Opcodes::NOT] = ops::Not;
        table[Opcodes::NEG] = ops::Neg;
        table[Opcodes::POP] = ops::Pop;
        table[Opcodes::DUP] = ops::Dup;
        table[Opcodes::DUP2] = ops::Dup2;
        table[Opcodes::DUP_X1] = ops::DupX1;
        table[Opcodes::DUP_X2] = ops::DupX2;
        table[Opcodes::SWAP] = ops::Swap;
        table[Opcodes::INC] = ops::Inc;
        table[Opcodes::LOAD] = ops::Load;
        table[Opcodes::STORE] = ops::Store;
        table[Opcodes::HLOAD] = ops::HLoad;
        table[Opcodes::HSTORE] = ops::HStore;
        table[Opcodes::RLOAD] = ops::RLoad;
        table[Opcodes::RSTORE] = ops::RStore;
        table[Opcodes::RLOAD_0] = ops::RLoad0;
        table[Opcodes::BALOAD] = ops::BALoad;
        table[Opcodes::BASTORE] = ops::BAStore;
        table[Opcodes::CALOAD] = ops::CALoad;
        table[Opcodes::CASTORE] = ops::CAStore;
        table[Opcodes::SALOAD] = ops::SALoad;
        table[Opcodes::SASTORE] = ops::SAStore;
        table[Opcodes::IALOAD] = ops::IALoad;
        table[Opcodes::IASTORE] = ops::IAStore;
        table[Opcodes::LALOAD] = ops::LALoad;
        table[Opcodes::LASTORE] = ops::LAStore;
        table[Opcodes::HALOAD] = ops::HALoad;
        table[Opcodes::HASTORE] = ops::HAStore;
        table[Opcodes::RALOAD] = ops::RALoad;
        table[Opcodes::RASTORE] = ops::RAStore;
        table[Opcodes::ARRAYLENGTH] = ops::ArrayLength;
        table[Opcodes::NEW] = ops::New;
        table[Opcodes::NEWARRAY] = ops::NewArray;
        table[Opcodes::RNEWARRAY] = ops::RNewArray;
        table[Opcodes::ISINSTANCE] = ops::IsInstance;
        table[Opcodes::GETFIELD] = ops::GetField;
        table[Opcodes::SETFIELD] = ops::SetField;
        table[Opcodes::GETGLOBAL] = ops::GetGlobal;
        table[Opcodes::SETGLOBAL] = ops::SetGlobal;
        table[Opcodes::JMP_CMPEQ] = ops::JmpCmpEq;
        table[Opcodes::JMP_CMPNE] = ops::JmpCmpNe;
        table[Opcodes::JMP_CMPLT] = ops::JmpCmpLt;
        table[Opcodes::JMP_CMPGT] = ops::JmpCmpGt;
        table[Opcodes::JMP_CMPLE] = ops::JmpCmpLe;
        table[Opcodes::JMP_CMPGE] = ops::JmpCmpGe;
        table[Opcodes::JMP_HCMPEQ] = ops::JmpHCmpEq;
        table[Opcodes::JMP_HCMPNE] = ops::JmpHCmpNe;
        table[Opcodes::JMP_RCMPEQ] = ops::JmpRCmpEq;
        table[Opcodes::JMP_RCMPNE] = ops::JmpRCmpNe;
        table[Opcodes::JMP_HNULL] = ops::JmpHNull;
        table[Opcodes::JMP_HNONNULL] = ops::JmpHNonNull;
        table[Opcodes::JMP_RNULL] = ops::JmpRNull;
        table[Opcodes::JMP_RNONNULL] = ops::JmpRNonNull;
        table[Opcodes::JMPEQ] = ops::JmpEq;
        table[Opcodes::JMPNE] = ops::JmpNe;
        table[Opcodes::JMPLT] = ops::JmpLt;
        table[Opcodes::JMPGT] = ops::JmpGt;
        table[Opcodes::JMPLE] = ops::JmpLe;
        table[Opcodes::JMPGE] = ops::JmpGe;
        table[Opcodes::JMP] = ops::Jmp;
        table[Opcodes::CMP] = ops::Cmp;
        table[Opcodes::HCMP] = ops::HCmp;
        table[Opcodes::RCMP] = ops::RCmp;
        table[Opcodes::BPUSH] = ops::BPush;
        table[Opcodes::SPUSH] = ops::SPush;
        table[Opcodes::IPUSH] = ops::IPush;
        table[Opcodes::LPUSH] = ops::LPush;
        table[Opcodes::L2B] = ops::L2B;
        table[Opcodes::L2S] = ops::L2S;
        table[Opcodes::L2I] = ops::L2I;
        table[Opcodes::CONST_M1] = ops::ConstM1;
        table[Opcodes::CONST_0] = ops::Const0;
        table[Opcodes::CONST_1] = ops::Const1;
        table[Opcodes::CONST_2] = ops::Const2;
        table[Opcodes::CONST_3] = ops::Const3;
        table[Opcodes::CONST_4] = ops::Const4;
        table[Opcodes::HCONST_NULL] = ops::HConstNull;
        table[Opcodes::RCONST_NULL] = ops::RConstNull;
        table[Opcodes::CALL] = ops::Call;
        table[Opcodes::CALLVIRTUAL] = ops::CallVirtual;
        table[Opcodes::RETURN] = ops::Return;
        table[Opcodes::LRETURN] = ops::LReturn;
        table[Opcodes::HRETURN] = ops::HReturn;
        table[Opcodes::RRETURN] = ops::RReturn;
        table[Opcodes::LDC] = ops::Ldc;
        table[Opcodes::WIDE] = ops::Wide;
    }

    namespace ops {
        void Add(Executor& executor) {
            i32 rhs = executor.getFrame()->pop();
            i32 lhs = executor.getFrame()->pop();

            executor.getFrame()->push(lhs + rhs);
        }

        void Sub(Executor& executor) {
            i32 rhs = executor.getFrame()->pop();
            i32 lhs = executor.getFrame()->pop();

            executor.getFrame()->push(lhs - rhs);
        }

        void Mul(Executor& executor) {
            i32 rhs = executor.getFrame()->pop();
            i32 lhs = executor.getFrame()->pop();

            executor.getFrame()->push(lhs * rhs);
        }

        void Div(Executor& executor) {
            i32 rhs = executor.getFrame()->pop();
            i32 lhs = executor.getFrame()->pop();

            executor.getFrame()->push(lhs / rhs);
        }

        void Rem(Executor& executor) {
            i32 rhs = executor.getFrame()->pop();
            i32 lhs = executor.getFrame()->pop();

            executor.getFrame()->push(lhs % rhs);
        }

        void And(Executor& executor) {
            i32 rhs = executor.getFrame()->pop();
            i32 lhs = executor.getFrame()->pop();

            executor.getFrame()->push(lhs & rhs);
        }

        void Or(Executor& executor) {
            i32 rhs = executor.getFrame()->pop();
            i32 lhs = executor.getFrame()->pop();

            executor.getFrame()->push(lhs | rhs);
        }

        void Xor(Executor& executor) {
            i32 rhs = executor.getFrame()->pop();
            i32 lhs = executor.getFrame()->pop();

            executor.getFrame()->push(lhs ^ rhs);
        }

        void Shl(Executor& executor) {
            i32 rhs = executor.getFrame()->pop();
            i32 lhs = executor.getFrame()->pop();

            executor.getFrame()->push(lhs << rhs);
        }

        void Shr(Executor& executor) {
            i32 rhs = executor.getFrame()->pop();
            i32 lhs = executor.getFrame()->pop();

            executor.getFrame()->push(lhs >> rhs);
        }

        void Not(Executor& executor) {
            i32 value = executor.getFrame()->pop();
            executor.getFrame()->push(~value);
        }

        void Neg(Executor& executor) {
            i32 value = executor.getFrame()->pop();
            executor.getFrame()->push(-value);
        }

        void Pop(Executor& executor) {
            executor.getFrame()->popGeneric();
        }

        void Dup(Executor& executor) {
            executor.getFrame()->dup();
        }

        void Dup2(Executor& executor) {
            executor.getFrame()->dup2()
        }

        void DupX1(Executor& executor) {
            executor.getFrame()->dupx1();
        }

        void DupX2(Executor& executor) {
            executor.getFrame()->dupx2();
        }

        void Swap(Executor& executor) {
            executor.getFrame()->swap();
        }

        void Inc(Executor& executor) {
            auto wide = executor.getWideGuard();
            u16 index = wide ? executor.fetchShort() : executor.fetch();
            i16 increment = wide ? executor.fetchShort() : executor.fetch();

            executor.getFrame()->incLocal(index, increment);
        }

        void Load(Executor& executor) {
            auto wide = executor.getWideGuard();
            u16 index = wide ? executor.fetchShort() : executor.fetch();

            executor.getFrame()->push(executor.getFrame()->getLocal(index));
        }

        void Store(Executor& executor) {
            auto wide = executor.getWideGuard();
            u16 index = wide ? executor.fetchShort() : executor.fetch();
            auto value = executor.getFrame()->pop();

            executor.getFrame()->setLocal(index, value);
        }

        void HLoad(Executor& executor) {
            auto wide = executor.getWideGuard();
            u16 index = wide ? executor.fetchShort() : executor.fetch();

            executor.getFrame()->pushHandle(executor.getFrame()->getLocalHandle(index));
        }

        void HStore(Executor& executor) {
            auto wide = executor.getWideGuard();
            u16 index = wide ? executor.fetchShort() : executor.fetch();
            auto value = executor.getFrame()->popHandle();

            executor.getFrame()->setLocalHandle(index, value);
        }

        void RLoad(Executor& executor) {
            auto wide = executor.getWideGuard();
            u16 index = wide ? executor.fetchShort() : executor.fetch();

            executor.getFrame()->pushObject(executor.getFrame()->getLocalObjectWeak(index));
        }

        void RStore(Executor& executor) {
            auto wide = executor.getWideGuard();
            u16 index = wide ? executor.fetchShort() : executor.fetch();
            auto value = executor.getFrame()->popObject();

            executor.getFrame()->setLocalObject(index, value);
        }

        void RLoad0(Executor& executor) {
            executor.getFrame()->pushObject(executor.getFrame()->getLocalObjectWeak(0));
        }

        void BALoad(Executor& executor) {
            Long index = executor.getFrame()->pop();
            ObjectRef array = executor.getFrame()->popObject();

            NullCheck(array);

            if (array->getClass()->getKind() != ClassKind::ARRAY) {
                std::cout << "reference of type " << array->getClass()->getName() << " is not an array reference\n";
                std::exit(1);
            }

            if (index < 0 || index >= array->getArrayLength()) {
                std::cout << "index " << index << " is out of bounds\n";
                std::exit(1);
            }

            auto elements = array->getArrayElements<Byte>();

            executor.getFrame()->push(elements[index]);
        }

        void BAStore(Executor& executor) {
            auto value = static_cast<Byte>(executor.getFrame()->pop());
            Long index = executor.getFrame()->pop();
            ObjectRef array = executor.getFrame()->popObject();

            NullCheck(array);

            if (array->getClass()->getKind() != ClassKind::ARRAY) {
                std::cout << "reference of type " << array->getClass()->getName() << " is not an array reference\n";
                std::exit(1);
            }

            if (index < 0 || index >= array->getArrayLength()) {
                std::cout << "index " << index << " is out of bounds\n";
                std::exit(1);
            }

            auto elements = array->getArrayElements<Byte>();

            elements[index] = value;
        }

        void CALoad(Executor& executor) {
            Long index = executor.getFrame()->pop();
            ObjectRef array = executor.getFrame()->popObject();

            NullCheck(array);

            if (array->getClass()->getKind() != ClassKind::ARRAY) {
                std::cout << "reference of type " << array->getClass()->getName() << " is not an array reference\n";
                std::exit(1);
            }

            if (index < 0 || index >= array->getArrayLength()) {
                std::cout << "index " << index << " is out of bounds\n";
                std::exit(1);
            }

            auto elements = array->getArrayElements<Char>();

            executor.getFrame()->push(elements[index]);
        }

        void CAStore(Executor& executor) {
            auto value = static_cast<Char>(executor.getFrame()->pop());
            Long index = executor.getFrame()->pop();
            ObjectRef array = executor.getFrame()->popObject();

            NullCheck(array);

            if (array->getClass()->getKind() != ClassKind::ARRAY) {
                std::cout << "reference of type " << array->getClass()->getName() << " is not an array reference\n";
                std::exit(1);
            }

            if (index < 0 || index >= array->getArrayLength()) {
                std::cout << "index " << index << " is out of bounds\n";
                std::exit(1);
            }

            auto elements = array->getArrayElements<Char>();

            elements[index] = value;
        }

        void SALoad(Executor& executor) {
            Long index = executor.getFrame()->pop();
            ObjectRef array = executor.getFrame()->popObject();

            NullCheck(array);

            if (array->getClass()->getKind() != ClassKind::ARRAY) {
                std::cout << "reference of type " << array->getClass()->getName() << " is not an array reference\n";
                std::exit(1);
            }

            if (index < 0 || index >= array->getArrayLength()) {
                std::cout << "index " << index << " is out of bounds\n";
                std::exit(1);
            }

            auto elements = array->getArrayElements<Short>();

            executor.getFrame()->push(elements[index]);
        }

        void SAStore(Executor& executor) {
            auto value = static_cast<Short>(executor.getFrame()->pop());
            Long index = executor.getFrame()->pop();
            ObjectRef array = executor.getFrame()->popObject();

            NullCheck(array);

            if (array->getClass()->getKind() != ClassKind::ARRAY) {
                std::cout << "reference of type " << array->getClass()->getName() << " is not an array reference\n";
                std::exit(1);
            }

            if (index < 0 || index >= array->getArrayLength()) {
                std::cout << "index " << index << " is out of bounds\n";
                std::exit(1);
            }

            auto elements = array->getArrayElements<Short>();

            elements[index] = value;
        }

        void IALoad(Executor& executor) {
            Long index = executor.getFrame()->pop();
            ObjectRef array = executor.getFrame()->popObject();

            NullCheck(array);

            if (array->getClass()->getKind() != ClassKind::ARRAY) {
                std::cout << "reference of type " << array->getClass()->getName() << " is not an array reference\n";
                std::exit(1);
            }

            if (index < 0 || index >= array->getArrayLength()) {
                std::cout << "index " << index << " is out of bounds\n";
                std::exit(1);
            }

            auto elements = array->getArrayElements<Int>();

            executor.getFrame()->push(elements[index]);
        }

        void IAStore(Executor& executor) {
            auto value = static_cast<Int>(executor.getFrame()->pop());
            Long index = executor.getFrame()->pop();
            ObjectRef array = executor.getFrame()->popObject();

            NullCheck(array);

            if (array->getClass()->getKind() != ClassKind::ARRAY) {
                std::cout << "reference of type " << array->getClass()->getName() << " is not an array reference\n";
                std::exit(1);
            }

            if (index < 0 || index >= array->getArrayLength()) {
                std::cout << "index " << index << " is out of bounds\n";
                std::exit(1);
            }

            auto elements = array->getArrayElements<Int>();

            elements[index] = value;
        }

        void LALoad(Executor& executor) {
            Long index = executor.getFrame()->pop();
            ObjectRef array = executor.getFrame()->popObject();

            NullCheck(array);

            if (array->getClass()->getKind() != ClassKind::ARRAY) {
                std::cout << "reference of type " << array->getClass()->getName() << " is not an array reference\n";
                std::exit(1);
            }

            if (index < 0 || index >= array->getArrayLength()) {
                std::cout << "index " << index << " is out of bounds\n";
                std::exit(1);
            }

            auto elements = array->getArrayElements<Long>();

            executor.getFrame()->push(elements[index]);
        }

        void LAStore(Executor& executor) {
            auto value = executor.getFrame()->pop();
            Long index = executor.getFrame()->pop();
            ObjectRef array = executor.getFrame()->popObject();

            NullCheck(array);

            if (array->getClass()->getKind() != ClassKind::ARRAY) {
                std::cout << "reference of type " << array->getClass()->getName() << " is not an array reference\n";
                std::exit(1);
            }

            if (index < 0 || index >= array->getArrayLength()) {
                std::cout << "index " << index << " is out of bounds\n";
                std::exit(1);
            }

            auto elements = array->getArrayElements<Long>();

            elements[index] = value;
        }

        void HALoad(Executor& executor) {
            Long index = executor.getFrame()->pop();
            ObjectRef array = executor.getFrame()->popObject();

            NullCheck(array);

            if (array->getClass()->getKind() != ClassKind::ARRAY) {
                std::cout << "reference of type " << array->getClass()->getName() << " is not an array reference\n";
                std::exit(1);
            }

            if (index < 0 || index >= array->getArrayLength()) {
                std::cout << "index " << index << " is out of bounds\n";
                std::exit(1);
            }

            auto elements = array->getArrayElements<Handle>();

            executor.getFrame()->pushHandle(elements[index]);
        }

        void HAStore(Executor& executor) {
            auto value = executor.getFrame()->popHandle();
            Long index = executor.getFrame()->pop();
            ObjectRef array = executor.getFrame()->popObject();

            NullCheck(array);

            if (array->getClass()->getKind() != ClassKind::ARRAY) {
                std::cout << "reference of type " << array->getClass()->getName() << " is not an array reference\n";
                std::exit(1);
            }

            if (index < 0 || index >= array->getArrayLength()) {
                std::cout << "index " << index << " is out of bounds\n";
                std::exit(1);
            }

            auto elements = array->getArrayElements<Handle>();

            elements[index] = value;
        }

        void RALoad(Executor& executor) {
            Long index = executor.getFrame()->pop();
            ObjectRef array = executor.getFrame()->popObject();

            NullCheck(array);

            if (array->getClass()->getKind() != ClassKind::ARRAY) {
                std::cout << "reference of type " << array->getClass()->getName() << " is not an array reference\n";
                std::exit(1);
            }

            if (index < 0 || index >= array->getArrayLength()) {
                std::cout << "index " << index << " is out of bounds\n";
                std::exit(1);
            }

            auto elements = array->getArrayElements<Object*>();

            executor.getFrame()->pushObject(elements[index]);
        }

        void RAStore(Executor& executor) {
            auto value = executor.getFrame()->popObject();
            Long index = executor.getFrame()->pop();
            ObjectRef array = executor.getFrame()->popObject();

            NullCheck(array);

            if (array->getClass()->getKind() != ClassKind::ARRAY) {
                std::cout << "reference of type " << array->getClass()->getName() << " is not an array reference\n";
                std::exit(1);
            }

            if (index < 0 || index >= array->getArrayLength()) {
                std::cout << "index " << index << " is out of bounds\n";
                std::exit(1);
            }

            auto elements = array->getArrayElements<Object*>();

            elements[index] = value;
            value->addReference();
        }

        void ArrayLength(Executor& executor) {
            ObjectRef array = executor.getFrame()->popObject();

            NullCheck(array);

            if (array->getClass()->getKind() != ClassKind::ARRAY) {
                std::cout << "reference of type " << array->getClass()->getName() << " is not an array reference\n";
                std::exit(1);
            }

            executor.getFrame()->push(array->getArrayLength());
        }

        void New(Executor& executor) {
            auto wide = executor.getWideGuard();
            u32 index = wide ? executor.fetchInt() : executor.fetchShort();
            auto classRef = executor.getFrame()->getConstPool().get<ConstantClass>(index);

            if (classRef == nullptr) {
                std::cout << "constant index " << index << " is out of bounds. todo: better errors\n";
                std::exit(1);
            }

            ObjectRef obj = AllocObject(classRef->getClass());

            executor.getFrame()->pushObject(obj);
        }

        void NewArray(Executor& executor) {
            u8 typeId = executor.fetch();
            Long size = executor.getFrame()->pop();

            ObjectRef array = AllocPrimitiveArray(typeId, size);

            executor.getFrame()->pushObject(array);
        }

        void RNewArray(Executor& executor) {
            auto wide = executor.getWideGuard();
            u32 index = wide ? executor.fetchInt() : executor.fetchShort();
            auto classRef = executor.getFrame()->getConstPool().get<ConstantClass>(index);

            if (classRef == nullptr) {
                std::cout << "constant index " << index << " is out of bounds\n";
                std::exit(1);
            }

            Long size = executor.getFrame()->pop();

            ObjectRef array = AllocArrayOf(classRef->getClass(), size);

            executor.getFrame()->pushObject(array);
        }

        void IsInstance(Executor& executor) {
            auto wide = executor.getWideGuard();
            u32 index = wide ? executor.fetchInt() : executor.fetchShort();
            auto classRef = executor.getFrame()->getConstPool().get<ConstantClass>(index);

            if (classRef == nullptr) {
                std::cout << "constant index " << index << " is out of bounds\n";
                std::exit(1);
            }

            ObjectRef object = executor.getFrame()->popObject();
            NullCheck(object);

            executor.getFrame()->push(object->isInstance(classRef->getClass()));
        }

        void GetField(Executor& executor) {
            auto wide = executor.getWideGuard();
            u32 index = wide ? executor.fetchInt() : executor.fetchShort();
            auto fieldRef = executor.getFrame()->getConstPool().get<ConstantField>(index);

            if (fieldRef == nullptr) {
                std::cout << "constant index " << index << " is out of bounds\n";
                std::exit(1);
            }

            ObjectRef object = executor.getFrame()->popObject();
            NullCheck(object);

            Field* field = fieldRef->getField();

            if (!object->isInstance(field->getClass())) {
                std::cout << "error: field '" << field->getType().getClassName() << " " << field->getName() << "' is not a part of the class " <<
                        object->getClass()->getModule()->getName() << ":" << object->getClass()->getName() << "\n";
                std::exit(1);
            }

            Stack::Frame* frame = executor.getFrame();

            switch (field->getType().getInternalType()) {
                case Type::REFERENCE:
                    frame->pushObject(object->getObjectWeak(field));
                    break;
                case Type::HANDLE:
                    frame->pushHandle(object->getHandle(field));
                    break;
                case Type::BYTE:
                    frame->push(object->getByte(field));
                    break;
                case Type::SHORT:
                    frame->push(object->getShort(field));
                    break;
                case Type::INT:
                    frame->push(object->getInt(field));
                    break;
                case Type::LONG:
                    frame->push(object->getLong(field));
                    break;
                case Type::CHAR:
                    frame->push(object->getChar(field));
                    break;
                case Type::FLOAT:
                    break;
                case Type::DOUBLE:
                    break;
                case Type::BOOL:
                    frame->push(object->getBool(field));
                    break;
            }
        }

        void SetField(Executor& executor) {
            auto wide = executor.getWideGuard();
            u32 index = wide ? executor.fetchInt() : executor.fetchShort();
            auto fieldRef = executor.getFrame()->getConstPool().get<ConstantField>(index);

            if (fieldRef == nullptr) {
                std::cout << "constant index " << index << " is out of bounds\n";
                std::exit(1);
            }

            ObjectRef object;
            Field* field = fieldRef->getField();

            auto getObject = [&executor, &object, field]() {
                object = executor.getFrame()->popObject();
                NullCheck(object);

                if (!object->isInstance(field->getClass())) {
                    std::cout << "error: field '" << field->getType().getClassName() << " " << field->getName() << "' is not a part of the class " <<
                            object->getClass()->getModule()->getName() << ":" << object->getClass()->getName() << "\n";
                    std::exit(1);
                }
            };

            Stack::Frame* frame = executor.getFrame();

            switch (field->getType().getInternalType()) {
                case Type::REFERENCE: {
                    ObjectRef value = frame->popObject();
                    getObject();

                    object->setObject(field, value);
                    break;
                }
                case Type::HANDLE: {
                    Handle value = frame->popHandle();
                    getObject();

                    object->setHandle(field, value);
                    break;
                }
                case Type::BYTE: {
                    Byte value = static_cast<Byte>(frame->pop());
                    getObject();

                    object->setByte(field, value);
                    break;
                }
                case Type::SHORT: {
                    Short value = static_cast<Short>(frame->pop());
                    getObject();

                    object->setShort(field, value);
                    break;
                }
                case Type::INT: {
                    Int value = frame->pop();
                    getObject();

                    object->setInt(field, value);
                    break;
                }
                case Type::LONG: {
                    Long value = frame->pop();
                    getObject();

                    object->setLong(field, value);
                    break;
                }
                case Type::CHAR: {
                    Char value = static_cast<Char>(frame->pop());
                    getObject();

                    object->setChar(field, value);
                    break;
                }
                case Type::FLOAT:
                    break;
                case Type::DOUBLE:
                    break;
                case Type::BOOL: {
                    Bool value = static_cast<Bool>(frame->pop());
                    getObject();

                    object->setBool(field, value);
                    break;
                }
            }
        }

        void GetGlobal(Executor& executor) {
            auto wide = executor.getWideGuard();
            u32 index = wide ? executor.fetchInt() : executor.fetchShort();
            auto globalRef = executor.getFrame()->getConstPool().get<ConstantGlobalVar>(index);

            if (globalRef == nullptr) {
                std::cout << "constant index " << index << " is out of bounds\n";
                std::exit(1);
            }

            GlobalVar* global = globalRef->getGlobalVar();
            Stack::Frame* frame = executor.getFrame();

            switch (global->getType().getInternalType()) {
                case Type::REFERENCE:
                    frame->pushObject(reinterpret_cast<Object*>(global->getValue().R));
                    break;
                case Type::HANDLE:
                    frame->pushHandle(global->getValue().H);
                    break;
                case Type::BYTE:
                    frame->push(global->getValue().B);
                    break;
                case Type::SHORT:
                    frame->push(global->getValue().S);
                    break;
                case Type::INT:
                    frame->push(global->getValue().I);
                    break;
                case Type::LONG:
                    frame->push(global->getValue().L);
                    break;
                case Type::CHAR:
                    frame->push(global->getValue().C);
                    break;
                case Type::FLOAT:
                    break;
                case Type::DOUBLE:
                    break;
                case Type::BOOL:
                    frame->push(global->getValue().Z);
                    break;
            }
        }

        void SetGlobal(Executor& executor) {
            auto wide = executor.getWideGuard();
            u32 index = wide ? executor.fetchInt() : executor.fetchShort();
            auto globalRef = executor.getFrame()->getConstPool().get<ConstantGlobalVar>(index);

            if (globalRef == nullptr) {
                std::cout << "constant index " << index << " is out of bounds\n";
                std::exit(1);
            }

            GlobalVar* global = globalRef->getGlobalVar();
            Stack::Frame* frame = executor.getFrame();

            switch (global->getType().getInternalType()) {
                case Type::REFERENCE: {
                    if (Object* old = reinterpret_cast<Object*>(global->getValue().R); old != nullptr) {
                        old->removeReference();
                    }

                    ObjectRef value = frame->popObject();
                    global->setValue(value);

                    if (value != nullptr) {
                        value->addReference();
                    }

                    break;
                }
                case Type::HANDLE: {
                    Handle value = frame->popHandle();
                    global->setValue(value);
                    break;
                }
                case Type::BYTE: {
                    Byte value = static_cast<Byte>(frame->pop());
                    global->setValue(value);
                    break;
                }
                case Type::SHORT: {
                    Short value = static_cast<Short>(frame->pop());
                    global->setValue(value);
                    break;
                }
                case Type::INT: {
                    Int value = frame->pop();
                    global->setValue(value);
                    break;
                }
                case Type::LONG: {
                    Long value = frame->pop();
                    global->setValue(value);
                    break;
                }
                case Type::CHAR: {
                    Char value = static_cast<Char>(frame->pop());
                    global->setValue(value);
                    break;
                }
                case Type::FLOAT:
                    break;
                case Type::DOUBLE:
                    break;
                case Type::BOOL: {
                    Bool value = static_cast<Bool>(frame->pop());
                    global->setValue(value);
                    break;
                }
            }
        }

        void JmpCmpEq(Executor& executor) {
            i16 branch = executor.fetchShort();
            Long rhs = executor.getFrame()->pop();
            Long lhs = executor.getFrame()->pop();

            if (lhs == rhs) {
                executor.branch(branch);
            }
        }

        void JmpCmpNe(Executor& executor) {
            i16 branch = executor.fetchShort();
            Long rhs = executor.getFrame()->pop();
            Long lhs = executor.getFrame()->pop();

            if (lhs != rhs) {
                executor.branch(branch);
            }
        }

        void JmpCmpLt(Executor& executor) {
            i16 branch = executor.fetchShort();
            Long rhs = executor.getFrame()->pop();
            Long lhs = executor.getFrame()->pop();

            if (lhs < rhs) {
                executor.branch(branch);
            }
        }

        void JmpCmpGt(Executor& executor) {
            i16 branch = executor.fetchShort();
            Long rhs = executor.getFrame()->pop();
            Long lhs = executor.getFrame()->pop();

            if (lhs > rhs) {
                executor.branch(branch);
            }
        }

        void JmpCmpLe(Executor& executor) {
            i16 branch = executor.fetchShort();
            Long rhs = executor.getFrame()->pop();
            Long lhs = executor.getFrame()->pop();

            if (lhs <= rhs) {
                executor.branch(branch);
            }
        }

        void JmpCmpGe(Executor& executor) {
            i16 branch = executor.fetchShort();
            Long rhs = executor.getFrame()->pop();
            Long lhs = executor.getFrame()->pop();

            if (lhs >= rhs) {
                executor.branch(branch);
            }
        }

        void JmpHCmpEq(Executor& executor) {
            i16 branch = executor.fetchShort();
            Handle rhs = executor.getFrame()->popHandle();
            Handle lhs = executor.getFrame()->popHandle();

            if (lhs == rhs) {
                executor.branch(branch);
            }
        }

        void JmpHCmpNe(Executor& executor) {
            i16 branch = executor.fetchShort();
            Handle rhs = executor.getFrame()->popHandle();
            Handle lhs = executor.getFrame()->popHandle();

            if (lhs != rhs) {
                executor.branch(branch);
            }
        }

        void JmpRCmpEq(Executor& executor) {
            i16 branch = executor.fetchShort();
            ObjectRef rhs = executor.getFrame()->popObject();
            ObjectRef lhs = executor.getFrame()->popObject();

            if (lhs == rhs) {
                executor.branch(branch);
            }
        }

        void JmpRCmpNe(Executor& executor) {
            i16 branch = executor.fetchShort();
            ObjectRef rhs = executor.getFrame()->popObject();
            ObjectRef lhs = executor.getFrame()->popObject();

            if (lhs != rhs) {
                executor.branch(branch);
            }
        }

        void JmpHNull(Executor& executor) {
            i16 branch = executor.fetchShort();
            Handle value = executor.getFrame()->popHandle();

            if (value == nullptr) {
                executor.branch(branch);
            }
        }

        void JmpHNonNull(Executor& executor) {
            i16 branch = executor.fetchShort();
            Handle value = executor.getFrame()->popHandle();

            if (value != nullptr) {
                executor.branch(branch);
            }
        }

        void JmpRNull(Executor& executor) {
            i16 branch = executor.fetchShort();
            ObjectRef value = executor.getFrame()->popObject();

            if (value == nullptr) {
                executor.branch(branch);
            }
        }

        void JmpRNonNull(Executor& executor) {
            i16 branch = executor.fetchShort();
            ObjectRef value = executor.getFrame()->popObject();

            if (value != nullptr) {
                executor.branch(branch);
            }
        }

        void JmpEq(Executor& executor) {
            i16 branch = executor.fetchShort();
            Long value = executor.getFrame()->pop();

            if (value == 0) {
                executor.branch(branch);
            }
        }

        void JmpNe(Executor& executor) {
            i16 branch = executor.fetchShort();
            Long value = executor.getFrame()->pop();

            if (value != 0) {
                executor.branch(branch);
            }
        }

        void JmpLt(Executor& executor) {
            i16 branch = executor.fetchShort();
            Long value = executor.getFrame()->pop();

            if (value < 0) {
                executor.branch(branch);
            }
        }

        void JmpGt(Executor& executor) {
            i16 branch = executor.fetchShort();
            Long value = executor.getFrame()->pop();

            if (value > 0) {
                executor.branch(branch);
            }
        }

        void JmpLe(Executor& executor) {
            i16 branch = executor.fetchShort();
            Long value = executor.getFrame()->pop();

            if (value <= 0) {
                executor.branch(branch);
            }
        }

        void JmpGe(Executor& executor) {
            i16 branch = executor.fetchShort();
            Long value = executor.getFrame()->pop();

            if (value >= 0) {
                executor.branch(branch);
            }
        }

        void Jmp(Executor& executor) {
            i16 branch = executor.fetchShort();
            executor.branch(branch);
        }

        void Cmp(Executor& executor) {
            Long rhs = executor.getFrame()->pop();
            Long lhs = executor.getFrame()->pop();

            if (lhs < rhs) executor.getFrame()->push(-1);
            else if (lhs > rhs) executor.getFrame()->push(1);
            else executor.getFrame()->push(0);
        }

        void HCmp(Executor& executor) {
            Handle rhs = executor.getFrame()->popHandle();
            Handle lhs = executor.getFrame()->popHandle();

            if (lhs < rhs) executor.getFrame()->push(-1);
            else if (lhs > rhs) executor.getFrame()->push(1);
            else executor.getFrame()->push(0);
        }

        void RCmp(Executor& executor) {
            ObjectRef rhs = executor.getFrame()->popObject();
            ObjectRef lhs = executor.getFrame()->popObject();

            if (lhs.get() < rhs.get()) executor.getFrame()->push(-1);
            else if (lhs.get() > rhs.get()) executor.getFrame()->push(1);
            else executor.getFrame()->push(0);
        }

        void BPush(Executor& executor) {
            i8 value = executor.fetch();
            executor.getFrame()->push(value);
        }

        void SPush(Executor& executor) {
            i16 value = executor.fetchShort();
            executor.getFrame()->push(value);
        }

        void IPush(Executor& executor) {
            i32 value = executor.fetchInt();
            executor.getFrame()->push(value);
        }

        void LPush(Executor& executor) {
            i64 value = executor.fetchLong();
            executor.getFrame()->push(value);
        }

        void L2B(Executor& executor) {
            Long value = executor.getFrame()->pop();
            executor.getFrame()->push(static_cast<Byte>(value));
        }

        void L2S(Executor& executor) {
            Long value = executor.getFrame()->pop();
            executor.getFrame()->push(static_cast<Short>(value));
        }

        void L2I(Executor& executor) {
            Long value = executor.getFrame()->pop();
            executor.getFrame()->push(static_cast<Int>(value));
        }

        void ConstM1(Executor& executor) {
            executor.getFrame()->push(-1);
        }

        void Const0(Executor& executor) {
            executor.getFrame()->push(0);
        }

        void Const1(Executor& executor) {
            executor.getFrame()->push(1);
        }

        void Const2(Executor& executor) {
            executor.getFrame()->push(2);
        }

        void Const3(Executor& executor) {
            executor.getFrame()->push(3);
        }

        void Const4(Executor& executor) {
            executor.getFrame()->push(4);
        }

        void HConstNull(Executor& executor) {
            executor.getFrame()->pushHandle(nullptr);
        }

        void RConstNull(Executor& executor) {
            executor.getFrame()->pushObject(nullptr);
        }

        void Call(Executor& executor) {
            auto wide = executor.getWideGuard();
            u32 index = wide ? executor.fetchInt() : executor.fetchShort();
            auto functionRef = executor.getFrame()->getConstPool().get<ConstantFunc>(index);

            if (functionRef == nullptr) {
                std::cout << "constant index " << index << " is out of bounds\n";
                std::exit(1);
            }

            Function* func = functionRef->getFunction();
            func->call(executor);
        }

        void CallVirtual(Executor& executor) {
            auto wide = executor.getWideGuard();
            u32 index = wide ? executor.fetchInt() : executor.fetchShort();
            auto methodRef = executor.getFrame()->getConstPool().get<ConstantMethod>(index);

            if (methodRef == nullptr) {
                std::cout << "constant index " << index << " is out of bounds\n";
                std::exit(1);
            }

            Method* method = methodRef->getMethod();
            Object* object = executor.getFrame()->extractThis(method);

            if (!object->isInstance(method->getClass())) {
                std::cout << "class " << object->getClass()->getName() << " is not assignable to " << method->getClass()->getName() << "\n";
                std::exit(1);
            }

            Function* func = object->getClass()->dispatchMethod(method);

            func->call(executor);
        }

        void Return(Executor& executor) {
            executor.leaveFunction();
        }

        void LReturn(Executor& executor) {
            Long value = executor.getFrame()->pop();

            executor.leaveFunction();
            executor.returnValue().L = value;

            if (Stack::Frame* frame = executor.getFrame(); frame != nullptr) {
                frame->push(value);
            }
        }

        void HReturn(Executor& executor) {
            Handle value = executor.getFrame()->popHandle();

            executor.leaveFunction();
            executor.returnValue().H = value;

            if (Stack::Frame* frame = executor.getFrame(); frame != nullptr) {
                frame->pushHandle(value);
            }
        }

        void RReturn(Executor& executor) {
            ObjectRef value = executor.getFrame()->popObject();

            executor.leaveFunction();
            executor.returnValue().R = value;

            if (Stack::Frame* frame = executor.getFrame(); frame != nullptr) {
                frame->pushObject(value);
            } else {
                value->addReference(); // maybe add it to the cycle collector?
            }
        }

        void Ldc(Executor& executor) {
            auto wide = executor.getWideGuard();
            u32 index = wide ? executor.fetchInt() : executor.fetchShort();
            Constant* value = executor.getFrame()->getConstPool().getGeneric(index);

            if (value == nullptr) {
                std::cout << "constant index " << index << " is out of bounds\n";
                std::exit(1);
            }

            value->addTo(executor.getFrame());
        }

        void Wide(Executor& executor) {
            executor.setWide(true);
        }
    }
}
