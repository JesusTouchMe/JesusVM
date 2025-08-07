// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_RUNTIME_STD_ARRAYS_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_RUNTIME_STD_ARRAYS_H 1

#include "JesusVM/Module.h"

namespace JesusVM::rt::std::Arrays {
    extern Module* self;

    extern Function* copy;

    void copy_impl(VMContext* ctx, JObject srcArr, Long srcPos, JObject destArr, Long destPos, Long count);
}

#endif // JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_RUNTIME_STD_ARRAYS_H