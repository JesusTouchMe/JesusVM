#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_MODULE_INFO_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_MODULE_INFO_H

#include "moduleweb/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_module_info {
    u32 magic;
    u16 bytecode_version;

} moduleweb_module_info;

#ifdef __cplusplus
}
#endif

#endif //JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_MODULE_INFO_H
