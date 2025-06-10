// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_METHOD_INFO_H
#define JESUSVM_MODULEWEB_INCLUDE_METHOD_INFO_H 1

#include "moduleweb/attribute_info.h"

#define MODULEWEB_METHOD_MODIFIER_PUBLIC    0x0001
#define MODULEWEB_METHOD_MODIFIER_PRIVATE   0x0002
#define MODULEWEB_METHOD_MODIFIER_PROTECTED 0x0004
#define MODULEWEB_METHOD_MODIFIER_FINAL     0x0008

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_method_info {
    u16 modifiers;
    u16 name_index; // CONSTANT_TYPE_NAME
    u16 function_index; // CONSTANT_TYPE_FUNCTION_REF
    moduleweb_attribute_array attributes;
} moduleweb_method_info;

int moduleweb_method_info_init(moduleweb_method_info* info, PARAM_MUTATED moduleweb_instream* stream);
void moduleweb_method_info_uninit(moduleweb_method_info* info);

int moduleweb_method_info_emit_bytes(moduleweb_method_info* info, PARAM_MUTATED moduleweb_outstream* stream);
void moduleweb_method_info_print(moduleweb_method_info* info, const moduleweb_module_info* module, u32 indent);

#ifdef __cplusplus
}
#endif

#endif //JESUSVM_MODULEWEB_INCLUDE_METHOD_INFO_H