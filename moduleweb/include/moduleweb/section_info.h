#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_SECTION_INFO_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_SECTION_INFO_H

#include "moduleweb/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_section_info {
    u32 size;
    u32 name_index;
    u8* bytes;
} moduleweb_section_info;

#ifdef __cplusplus
}
#endif

#endif //JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_SECTION_INFO_H
