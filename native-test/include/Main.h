// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_NATIVE_TEST_INCLUDE_MAIN_H
#define JESUSVM_NATIVE_TEST_INCLUDE_MAIN_H 1

#include "JesusVM/JesusNative.h"

#ifdef __cplusplus
extern "C" {
#endif

JESUSVM_EXPORT void JESUSVM_CALL JesusVM_Main_print__I(VMContext ctx, JValue* args, Int value);

#ifdef __cplusplus
}
#endif

#endif // JESUSVM_NATIVE_TEST_INCLUDE_MAIN_H
