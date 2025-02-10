// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_FUNCTION_BUILDER_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_FUNCTION_BUILDER_H 1

#include "moduleweb/module_info.h"

#include "moduleweb/builder/attribute_builder.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_module_builder moduleweb_module_builder;

typedef struct moduleweb_function {
    u16 modifiers;
    OWNED_OBJECT char* name;
    OWNED_OBJECT char* descriptor;

    moduleweb_attribute_vector attributes;
} moduleweb_function;

typedef struct moduleweb_function_builder {
    UNOWNED_OBJECT moduleweb_module_builder* module;

    u16 modifiers;
    OWNED_OBJECT char* name;
    OWNED_OBJECT char* descriptor;

    moduleweb_attribute_vector attributes;
} moduleweb_function_builder;

void moduleweb_function_delete(moduleweb_function* function);

/**
 * Will free all objects owned by the builder (in case it hasn't built)
 */
void moduleweb_function_builder_delete(moduleweb_function_builder* builder);

void moduleweb_function_builder_modifiers(moduleweb_function_builder* builder, u16 modifiers);

void moduleweb_function_builder_descriptor(moduleweb_function_builder* builder, PARAM_COPIED const char* name, PARAM_COPIED const char* descriptor);

void moduleweb_function_builder_add_attribute(moduleweb_function_builder* builder, moduleweb_attribute* attribute);

moduleweb_function* moduleweb_function_builder_build(moduleweb_function_builder* builder);

#ifdef __cplusplus
}
#endif

#endif // JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_FUNCTION_BUILDER_H
