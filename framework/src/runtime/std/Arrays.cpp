// Copyright 2025 JesusTouchMe

#include "JesusVM/runtime/std/Arrays.h"

#include "JesusVM/JesusVM.h"

#include <cstring>

#undef BOOL

namespace JesusVM::rt::std::Arrays {
    Module* self;

    Function* copy;

    void copy_impl(VMContext* ctx, JObject srcArr, Long srcPos, JObject destArr, Long destPos, Long count) {
        Object* src = reinterpret_cast<Object*>(srcArr);
        Object* dest = reinterpret_cast<Object*>(destArr);

        if (src == nullptr) Trap(CreateString("Arrays::copy: srcArr is null"), "<native>", -1, -1);
        if (dest == nullptr) Trap(CreateString("Arrays::copy: destArr is null"), "<native>", -1, -1);

        if (src->getClass()->getKind() != ClassKind::ARRAY) Trap(CreateString("Arrays::copy: srcArr isn't an array"), "<native>", -1, -1);
        if (dest->getClass()->getKind() != ClassKind::ARRAY) Trap(CreateString("Arrays::copy: destArr isn't an array"), "<native>", -1, -1);

        if (srcPos < 0 || destPos < 0 || count < 0 || srcPos + count > src->getArrayLength() || destPos + count > dest->getArrayLength())
            Trap(CreateString("Arrays::copy: Index out of bounds"), "<native>", -1, -1);

        if (src->getClass() != dest->getClass()) Trap(CreateString("Arrays::copy: srcArr and destArr types mismatch"), "<native>", -1, -1);

        auto copyPrimitive = [src, dest, srcPos, destPos, count](size_t elementSize) {
            char* srcPtr = src->getArrayElements<char>() + srcPos * elementSize;
            char* destPtr = dest->getArrayElements<char>() + destPos * elementSize;

            memmove(destPtr, srcPtr, count * elementSize);
        };

        Class* baseClass = src->getClass()->getArrayBaseClass();
        if (baseClass->getKind() == ClassKind::PRIMITIVE) {
            switch (baseClass->getRepresentedPrimitive()) {
                case Type::HANDLE:
                    copyPrimitive(sizeof(Handle));
                    return;
                case Type::BYTE:
                    copyPrimitive(sizeof(Byte));
                    return;
                case Type::SHORT:
                    copyPrimitive(sizeof(Short));
                    return;
                case Type::INT:
                    copyPrimitive(sizeof(Int));
                    return;
                case Type::LONG:
                    copyPrimitive(sizeof(Long));
                    return;
                case Type::CHAR:
                    copyPrimitive(sizeof(Char));
                    return;
                case Type::FLOAT:
                    copyPrimitive(sizeof(Float));
                    return;
                case Type::DOUBLE:
                    copyPrimitive(sizeof(Double));
                    return;
                case Type::BOOL:
                    copyPrimitive(sizeof(Bool));
                    return;

                default:
                    Trap(CreateString("Arrays::copy: received impossible primitive class"), "<native>", -1, -1);
            }
        } else { // array and standard... stored as the same type
            Object** srcData = src->getArrayElements<Object*>();
            Object** destData = dest->getArrayElements<Object*>();

            bool overlapping = (srcData == destData) && (destPos > srcPos && destPos < srcPos + count);

            if (overlapping) {
                for (Long i = count - 1; i >= 0; i--) {
                    Object* obj = src->getArrayElements<Object*>()[srcPos + i];
                    Object*& destSlot = dest->getArrayElements<Object*>()[destPos + i];

                    if (destSlot == obj) continue;
                    if (obj) obj->addReference();
                    if (destSlot) destSlot->removeReference();

                    destSlot = obj;
                }
            } else {
                for (Long i = 0; i < count; i++) {
                    Object* obj = src->getArrayElements<Object*>()[srcPos + i];
                    Object*& destSlot = dest->getArrayElements<Object*>()[destPos + i];

                    if (destSlot == obj) continue;
                    if (obj) obj->addReference();
                    if (destSlot) destSlot->removeReference();

                    destSlot = obj;
                }
            }
        }
    }
}
