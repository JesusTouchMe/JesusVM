// Copyright 2025 JesusTouchMe

#include "Main.h"

#include <iostream>

extern "C" JESUSVM_EXPORT void JESUSVM_CALL JesusVM_std_IO_print__I(VMContext ctx, JValue* args, Int value) {
    value = args[0].I;

    std::cout << value << std::endl;
}