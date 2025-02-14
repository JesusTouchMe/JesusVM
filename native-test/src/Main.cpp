// Copyright 2025 JesusTouchMe

#include "Main.h"

#include <iostream>

extern "C" JESUSVM_EXPORT void JESUSVM_CALL JesusVM_Main_print__I(VMContext ctx, JValue* args, Int value) {
    EXTRACT_ARG(I, value);

    std::cout << value << std::endl;
}