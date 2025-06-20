// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_RUNTIME_STD_PRIMITIVES_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_RUNTIME_STD_PRIMITIVES_H

#include "JesusVM/Module.h"

namespace JesusVM::rt::std::Primitives {
    extern Module* self;

    namespace Object {
        extern Class* self;
    }

    namespace String {
        extern Class* self;

        extern Field* data;

        extern Function* init;
    }
}

#endif //JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_RUNTIME_STD_PRIMITIVES_H
