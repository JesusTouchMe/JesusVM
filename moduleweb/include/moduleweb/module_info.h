#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_MODULE_INFO_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_MODULE_INFO_H

#include "moduleweb/class_info.h"
#include "moduleweb/constant_info.h"
#include "moduleweb/function_info.h"
#include "moduleweb/global_var_info.h"

#define MODULEWEB_MAGIC_NUMBER 0xFACC6969

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_module_info {
    u32 magic;
    u16 bytecode_version;

    char* name;

    moduleweb_attribute_array attributes;

    u16 constant_pool_size;
    OWNED_OBJECT moduleweb_constant_info* constant_pool; // [constant_pool_size]

    u16 class_count;
    OWNED_OBJECT moduleweb_class_info* classes;

    u16 global_var_count;
    OWNED_OBJECT moduleweb_global_var_info* global_vars;

    u16 function_count;
    OWNED_OBJECT moduleweb_function_info* functions;
} moduleweb_module_info;

int moduleweb_module_info_init(moduleweb_module_info* info, PARAM_COPIED const char* name, u64 name_length, PARAM_MUTATED moduleweb_instream* stream);
void moduleweb_module_info_uninit(moduleweb_module_info* info);

int moduleweb_module_info_emit_bytes(moduleweb_module_info* info, PARAM_MUTATED moduleweb_outstream* stream);
void moduleweb_module_info_print(moduleweb_module_info* info, u32 indent);

int moduleweb_module_constant_get_ascii(const moduleweb_module_info* module, u16 i, PARAM_MUTATED moduleweb_constant_ascii_info** result);
int moduleweb_module_constant_get_name(const moduleweb_module_info* module, u16 i, PARAM_MUTATED moduleweb_constant_name_info** result);
int moduleweb_module_constant_get_module_ref(const moduleweb_module_info* module, u16 i, PARAM_MUTATED moduleweb_constant_module_ref_info** result);
int moduleweb_module_constant_get_global_var_ref(const moduleweb_module_info* module, u16 i, PARAM_MUTATED moduleweb_constant_global_var_ref_info** result);
int moduleweb_module_constant_get_function_ref(const moduleweb_module_info* module, u16 i, PARAM_MUTATED moduleweb_constant_function_ref_info** result);
int moduleweb_module_constant_get_class_ref(const moduleweb_module_info* module, u16 i, PARAM_MUTATED moduleweb_constant_class_ref_info** result);
int moduleweb_module_constant_get_field_ref(const moduleweb_module_info* module, u16 i, PARAM_MUTATED moduleweb_constant_field_ref_info** result);

char* moduleweb_module_constant_to_string(const moduleweb_module_info* module, u16 i, PARAM_MUTATED u64* length);

#ifdef __cplusplus
}
#endif

#endif //JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_MODULE_INFO_H
