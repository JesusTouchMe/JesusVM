// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_MODULE_BUILDER_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_MODULE_BUILDER_H 1

#include "moduleweb/module_info.h"

#include "moduleweb/builder/class_builder.h"
#include "moduleweb/builder/function_builder.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_constant {
    u16 index;
    moduleweb_constant_info info;
} moduleweb_constant;

typedef struct moduleweb_module_builder {
    u16 bytecode_version;
    OWNED_OBJECT char* name;

    moduleweb_attribute_array attributes;

    // keep a separate pool for each constant type so we can search super duper fast !
    u16 constant_pool_size[MODULEWEB_CONSTANT_TYPE_AMOUNT];
    u16 constant_pool_capacity[MODULEWEB_CONSTANT_TYPE_AMOUNT];
    u16 constant_pool_index;
    OWNED_OBJECT moduleweb_constant* constant_pool[MODULEWEB_CONSTANT_TYPE_AMOUNT];

    u16 class_size;
    u16 class_capacity;
    OWNED_OBJECT moduleweb_class* classes;

    u16 function_size;
    u16 function_capacity;
    OWNED_OBJECT moduleweb_function* functions;
} moduleweb_module_builder;

/**
 * Will free all objects owned by the builder (in case it hasn't built)
 */
void moduleweb_module_builder_delete(moduleweb_module_builder* builder);

void moduleweb_module_builder_version(moduleweb_module_builder* builder, u16 version);

void moduleweb_module_builder_name(moduleweb_module_builder* builder, PARAM_COPIED const char* name);

void moduleweb_module_builder_add_attribute(moduleweb_module_builder* builder, PARAM_MOVED moduleweb_attribute* attribute);

void moduleweb_module_builder_add_class(moduleweb_module_builder* builder, PARAM_MOVED moduleweb_class* clas);

void moduleweb_module_builder_add_function(moduleweb_module_builder* builder, PARAM_MOVED moduleweb_class* clas);

void moduleweb_module_builder_build(moduleweb_module_builder* builder, PARAM_MUTATED moduleweb_module_info* info);

#ifdef __cplusplus
}
#endif

#endif // JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_MODULE_BUILDER_H
