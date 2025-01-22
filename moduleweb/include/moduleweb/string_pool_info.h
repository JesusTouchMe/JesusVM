// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_STRING_POOL_INFO_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_STRING_POOL_INFO_H 1

#include "moduleweb/stream.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_string_view {
    u32 size;
    char* ascii;
} moduleweb_string_view;

typedef struct moduleweb_string_pool_info { // not necessarily for human strings too. used for all sets of bytes such as those used for attributes
    u32 total_size;
    u8* bytes;
} moduleweb_string_pool_info;

int moduleweb_string_pool_info_init(moduleweb_string_pool_info* info, moduleweb_instream* stream);
void moduleweb_string_pool_info_uninit(moduleweb_string_pool_info* info);

int moduleweb_string_pool_info_get_string(moduleweb_string_pool_info* info, moduleweb_string_view* string_view, u32 index);

#ifdef __cplusplus
}
#endif

#endif // JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_STRING_POOL_INFO_H
