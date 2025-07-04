// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_GC_REFERENCES_H
#define JESUSVM_GC_REFERENCES_H 1

#include "JesusVM/heap/Object.h"

#include <functional>

// Exclusively used by native function shenanigans

namespace JesusVM::GC {
    Handle CreateGlobalReference(Object* object);
    void DeleteGlobalReference(Handle reference);
    Handle CreateGlobalSpecial(void* ptr, std::function<void(void*)> destructor);
    void DeleteGlobalSpecial(Handle object);

    Handle CreateGlobalWeakReference(Object* object);
    void DeleteGlobalWeakReference(Handle reference);
    Handle CreateGlobalWeakSpecial(void* ptr, std::function<void(void*)> destructor);
    void DeleteGlobalWeakSpecial(Handle object);

    VMErr PushLocalFrame(Int initialCapacity);
    VMErr PopLocalFrame();

    Handle CreateLocalReference(Object* object);
    void DeleteLocalReference(Handle reference);
    Handle CreateLocalSpecial(void* ptr, std::function<void(void*)> destructor);
    void DeleteLocalSpecial(Handle object);

    void ReleaseWeakReferences(size_t byteLimit);
}

#endif // JESUSVM_GC_REFERENCES_H
