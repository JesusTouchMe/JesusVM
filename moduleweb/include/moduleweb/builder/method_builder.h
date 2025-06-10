// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_METHOD_BUILDER_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_METHOD_BUILDER_H 1

#include "moduleweb/builder/function_builder.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_module_builder moduleweb_module_builder;

typedef struct moduleweb_method {
    u16 modifiers;
    OWNED_OBJECT char* name;
    OWNED_OBJECT char* descriptor;

    UNOWNED_OBJECT moduleweb_function* function;

    moduleweb_attribute_vector attributes;
} moduleweb_method;

typedef struct moduleweb_method_builder {
    UNOWNED_OBJECT moduleweb_module_builder* module;

    u16 modifiers;
    OWNED_OBJECT char* name;
    OWNED_OBJECT char* descriptor;

   UNOWNED_OBJECT moduleweb_function* function;

    moduleweb_attribute_vector attributes;
} moduleweb_method_builder;

void moduleweb_method_delete(moduleweb_method* method);

/**
 * Will delete all objects owned by the builder (in case it hasn't built)
 */
void moduleweb_method_builder_delete(moduleweb_method_builder* builder);

void moduleweb_method_builder_modifiers(moduleweb_method_builder* builder, u16 modifiers);

void moduleweb_method_builder_descriptor(moduleweb_method_builder* builder, PARAM_COPIED const char* name, PARAM_COPIED const char* descriptor);

void moduleweb_method_builder_function(moduleweb_method_builder* builder, moduleweb_function* function);

void moduleweb_method_builder_add_attribute(moduleweb_method_builder* builder, moduleweb_attribute* attribute);

moduleweb_method* moduleweb_method_builder_build(moduleweb_method_builder* builder);

#ifdef __cplusplus
}
#endif

#endif //JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_METHOD_BUILDER_H