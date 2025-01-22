#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_MODULE_INFO_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_MODULE_INFO_H

#include "moduleweb/constant_info.h"
#include "moduleweb/string_pool_info.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_module_info {
    u32 magic;
    u16 bytecode_version;
    u16 name_index;

    moduleweb_string_pool_info string_pool;

    u16 constant_pool_size;
    moduleweb_constant_info* constant_pool; // [constant_pool_size]

    u16 class_count;
} moduleweb_module_info;

#ifdef __cplusplus
}
#endif

#endif //JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_MODULE_INFO_H
