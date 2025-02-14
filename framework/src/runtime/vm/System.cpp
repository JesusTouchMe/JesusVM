// Copyright 2025 JesusTouchMe

#include "JesusVM/runtime/vm/System.h"

namespace JesusVM::rt::vm::System {
    Module* self;

    Class* voidClass;
    Class* boolClass;
    Class* byteClass;
    Class* shortClass;
    Class* intClass;
    Class* longClass;
    Class* charClass;
    Class* floatClass;
    Class* doubleClass;
    Class* handleClass;

    Class* boolArray;
    Class* byteArray;
    Class* shortArray;
    Class* intArray;
    Class* longArray;
    Class* charArray;
    Class* floatArray;
    Class* doubleArray;
    Class* handleArray;

    namespace Object {
        Class* self;
    }
}
