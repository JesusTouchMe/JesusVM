// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_ATTRIBUTE_INFO_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_ATTRIBUTE_INFO_H 1

#include "moduleweb/stream.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_attribute_info {
    u16 name_index;
    u32 info_index; // in string pool, treated as bytes or string (up to the attribute itself)
} moduleweb_attribute_info;

typedef struct moduleweb_attribute_array {
    u16 size;
    moduleweb_attribute_info* array;
} moduleweb_attribute_array;

int moduleweb_attribute_info_init(moduleweb_attribute_info* info, moduleweb_instream* stream);
void moduleweb_attribute_info_uninit(moduleweb_attribute_info* info);

int moduleweb_attribute_array_init(moduleweb_attribute_array* array, moduleweb_instream* stream);
void moduleweb_attribute_array_uninit(moduleweb_attribute_array* array);

#ifdef __cplusplus
}
#endif

#endif // JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_ATTRIBUTE_INFO_H
