// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_GLOBAL_VAR_INFO_H
#define JESUSVM_MODULEWEB_INCLUDE_GLOBAL_VAR_INFO_H 1

#include "moduleweb/attribute_info.h"

#define MODULEWEB_GLOBAL_VAR_MODIFIER_PUBLIC 0x0001
#define MODULEWEB_GLOBAL_VAR_MODIFIER_PRIVATE 0x0002
#define MODULEWEB_GLOBAL_VAR_MODIFIER_VOLATILE 0x0004

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_global_var_info {
    u16 modifiers;
    u16 name_index; // CONSTANT_TYPE_NAME
    moduleweb_attribute_array attributes;
} moduleweb_global_var_info;

int moduleweb_global_var_info_init(moduleweb_global_var_info* info, PARAM_MUTATED moduleweb_instream* stream);
void moduleweb_global_var_info_uninit(moduleweb_global_var_info* info);

int moduleweb_global_var_info_emit_bytes(moduleweb_global_var_info* info, PARAM_MUTATED moduleweb_outstream* stream);
void moduleweb_global_var_info_print(moduleweb_global_var_info* info, const moduleweb_module_info* module, u32 indent);

#ifdef __cplusplus
}
#endif

#endif // JESUSVM_MODULEWEB_INCLUDE_GLOBAL_VAR_INFO_H
