// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_GLOBAL_VAR_BUILDER_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_GLOBAL_VAR_BUILDER_H 1

#include "moduleweb/module_info.h"

#include "moduleweb/builder/attribute_builder.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_module_builder moduleweb_module_builder;

typedef struct moduleweb_global_var {
    u16 modifiers;
    OWNED_OBJECT char* name;
    OWNED_OBJECT char* descriptor;

    moduleweb_attribute_vector attributes;
} moduleweb_global_var;

typedef struct moduleweb_global_var_builder {
    UNOWNED_OBJECT moduleweb_module_builder* module;

    u16 modifiers;
    OWNED_OBJECT char* name;
    OWNED_OBJECT char* descriptor;

    moduleweb_attribute_vector attributes;
} moduleweb_global_var_builder;

void moduleweb_global_var_delete(moduleweb_global_var* global_var);

/**
 * Will delete all objects owned by the builder (in case it hasn't built)
 */
void moduleweb_global_var_builder_delete(moduleweb_global_var_builder* builder);

void moduleweb_global_var_builder_modifiers(moduleweb_global_var_builder* builder, u16 modifiers);

void moduleweb_global_var_builder_descriptor(moduleweb_global_var_builder* builder, PARAM_COPIED const char* name, PARAM_COPIED const char* descriptor);

void moduleweb_global_var_builder_add_attribute(moduleweb_global_var_builder* builder, moduleweb_attribute* attribute);

moduleweb_global_var* moduleweb_global_var_builder_build(moduleweb_global_var_builder* builder);

#ifdef __cplusplus
}
#endif

#endif // JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_GLOBAL_VAR_BUILDER_H
