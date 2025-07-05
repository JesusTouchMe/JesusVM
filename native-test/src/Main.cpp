// Copyright 2025 JesusTouchMe

#include "Main.h"

#include <iostream>

extern "C" JESUSVM_EXPORT void JesusVM_std_IO_print__I(VMContext* ctx, Int value) {
    std::cout << value << std::endl;
}