#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_CONSTANT_INFO_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_CONSTANT_INFO_H

#include "moduleweb/stream.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum moduleweb_constant_type : u8 {
    MODULEWEB_CONSTANT_TYPE_ASCII = 0,
    MODULEWEB_CONSTANT_TYPE_NAME = 1,
    MODULEWEB_CONSTANT_TYPE_MODULE_REF = 2,
    MODULEWEB_CONSTANT_TYPE_FUNCTION = 3
} moduleweb_constant_type;

typedef struct moduleweb_constant_ascii_info { // with this constant type, the size of all things that require names can be cut in half as they don't have to index into string pool
    u32 string_pool_index;
} moduleweb_constant_ascii_info;

typedef struct moduleweb_constant_name_info {
    u16 name_index;
    u16 descriptor_index;
} moduleweb_constant_name_info;

typedef struct moduleweb_constant_module_ref_info {
    u16 name_index;
} moduleweb_constant_module_ref_info;

typedef struct moduleweb_constant_function_info {
    u16 module_index;
    u16 name_info_index;
} moduleweb_constant_function_info;

typedef struct moduleweb_constant_info {
    moduleweb_constant_type type;
    union {
        moduleweb_constant_ascii_info ascii_info;
        moduleweb_constant_name_info name_info;
        moduleweb_constant_module_ref_info module_ref_info;
        moduleweb_constant_function_info function_info;
    };
} moduleweb_constant_info;

int moduleweb_constant_info_init(moduleweb_constant_info* info, moduleweb_instream* stream);

void moduleweb_constant_info_uninit(moduleweb_constant_info* info);

#ifdef __cplusplus
}
#endif

#endif //JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_CONSTANT_INFO_H
