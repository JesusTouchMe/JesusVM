// Copyright 2025 JesusTouchMe

#include "JesusVM/runtime/std/Primitives.h"

namespace JesusVM::rt::std::Primitives {
    Module* self;

    namespace Object {
        Class* self;
    }

    namespace String {
        Class* self;

        Field* data;

        Function* init;
    }
}