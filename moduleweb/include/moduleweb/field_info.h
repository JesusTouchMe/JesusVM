// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_FIELD_INFO_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_FIELD_INFO_H 1

#include "moduleweb/attribute_info.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_field_info {
    u16 modifiers;
    u16 name_index;
    u16 descriptor_index; // not using CONSTANT_TYPE_NAME to potentially save const pool space with useless stuff
    moduleweb_attribute_array attributes;
} moduleweb_field_info;

#ifdef __cplusplus
}
#endif

#endif // JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_FIELD_INFO_H
