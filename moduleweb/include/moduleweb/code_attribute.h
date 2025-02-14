// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_CODE_ATTRIBUTE_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_CODE_ATTRIBUTE_H 1

#include "moduleweb/stream.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_code_attribute {
    u16 max_locals;
    u16 max_stack_size;

    u32 code_length;
    u8* code;
} moduleweb_code_attribute;

int moduleweb_code_attribute_init(moduleweb_code_attribute* code, moduleweb_instream* stream);
void moduleweb_code_attribute_uninit(moduleweb_code_attribute* code);

#ifdef __cplusplus
}
#endif

#endif // JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_CODE_ATTRIBUTE_H
