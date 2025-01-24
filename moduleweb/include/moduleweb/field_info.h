// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_FIELD_INFO_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_FIELD_INFO_H 1

#include "moduleweb/attribute_info.h"

#define MODULEWEB_FIELD_MODIFIER_PUBLIC     0x0001
#define MODULEWEB_FIELD_MODIFIER_PRIVATE    0x0002
#define MODULEWEB_FIELD_MODIFIER_PROTECTED  0x0004
#define MODULEWEB_FIELD_MODIFIER_CONST      0x0008
#define MODULEWEB_FIELD_MODIFIER_VOLATILE   0x0010

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_field_info {
    u16 modifiers;
    u16 name_index; // CONSTANT_TYPE_NAME
    moduleweb_attribute_array attributes;
} moduleweb_field_info;

int moduleweb_field_info_init(moduleweb_field_info* info, moduleweb_instream* stream);
void moduleweb_field_info_uninit(moduleweb_field_info* info);

int moduleweb_field_info_emit_bytes(moduleweb_field_info* info, moduleweb_outstream* stream);
void moduleweb_field_info_print(moduleweb_field_info* info, const moduleweb_module_info* module, u32 indent);

#ifdef __cplusplus
}
#endif

#endif // JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_FIELD_INFO_H
