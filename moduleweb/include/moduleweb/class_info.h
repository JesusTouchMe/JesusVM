// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_CLASS_INFO_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_CLASS_INFO_H 1

#include "moduleweb/field_info.h"

#define MODULEWEB_CLASS_MODIFIER_PUBLIC     0x0001
#define MODULEWEB_CLASS_MODIFIER_PRIVATE    0x0002
#define MODULEWEB_CLASS_MODIFIER_ABSTRACT   0x0004
#define MODULEWEB_CLASS_MODIFIER_FINAL      0x0008

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_class_info {
    u16 name_index;
    u16 modifiers;

    u16 super_class;

    moduleweb_attribute_array attributes;

    u16 field_count;
    moduleweb_field_info* fields; // [field_count]
} moduleweb_class_info;

int moduleweb_class_info_init(moduleweb_class_info* info, moduleweb_instream* stream);
void moduleweb_class_info_uninit(moduleweb_class_info* info);

int moduleweb_class_info_emit_bytes(moduleweb_class_info* info, moduleweb_outstream* stream);
void moduleweb_class_info_print(moduleweb_class_info* info, const moduleweb_module_info* module, u32 indent);

int moduleweb_class_get_field(const moduleweb_class_info* info, const moduleweb_module_info* module, const char* name, const char* descriptor, moduleweb_field_info** result);

#ifdef __cplusplus
}
#endif

#endif // JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_CLASS_INFO_H
