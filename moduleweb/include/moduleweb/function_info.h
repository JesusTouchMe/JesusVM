#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_FUNCTION_INFO_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_FUNCTION_INFO_H

#include "moduleweb/attribute_info.h"

#define MODULEWEB_FUNCTION_MODIFIER_PUBLIC      0x0001
#define MODULEWEB_FUNCTION_MODIFIER_PRIVATE     0x0002
#define MODULEWEB_FUNCTION_MODIFIER_PURE        0x0004
#define MODULEWEB_FUNCTION_MODIFIER_ASYNC       0x0008
#define MODULEWEB_FUNCTION_MODIFIER_NATIVE      0x0010

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_function_info {
    u16 modifiers;
    u16 name_index; // CONSTANT_TYPE_NAME
    moduleweb_attribute_array attributes;
} moduleweb_function_info;

int moduleweb_function_info_init(moduleweb_function_info* info, moduleweb_instream* stream);
void moduleweb_function_info_uninit(moduleweb_function_info* info);

int moduleweb_function_info_emit_bytes(moduleweb_function_info* info, moduleweb_outstream* stream);
void moduleweb_function_info_print(moduleweb_function_info* info);

#ifdef __cplusplus
}
#endif

#endif //JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_FUNCTION_INFO_H
