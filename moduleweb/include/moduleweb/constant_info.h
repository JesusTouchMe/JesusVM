#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_CONSTANT_INFO_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_CONSTANT_INFO_H

#include "moduleweb/stream.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum moduleweb_constant_type : u8 {
    MODULEWEB_CONSTANT_TYPE_NAME,
    MODULEWEB_CONSTANT_TYPE_FUNCTION
} moduleweb_constant_type;

typedef struct moduleweb_constant_name_info {
    u32 name_index;
    u32 descriptor_index;
} moduleweb_constant_name_info;

typedef struct moduleweb_constant_function_info {
    u16 name_info_index;
} moduleweb_constant_function_info;

typedef struct moduleweb_constant_info {
    moduleweb_constant_type type;
    bool initialized;
    union {
        moduleweb_constant_name_info name_info;
        moduleweb_constant_function_info function_info;
    };
} moduleweb_constant_info;

int moduleweb_constant_info_init(moduleweb_constant_info* info, moduleweb_instream* stream);

void moduleweb_constant_info_uninit(moduleweb_constant_info* info);

#ifdef __cplusplus
}
#endif

#endif //JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_CONSTANT_INFO_H
