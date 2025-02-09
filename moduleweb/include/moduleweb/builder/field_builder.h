// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_FIELD_BUILDER_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_FIELD_BUILDER_H 1

#include "moduleweb/module_info.h"

#include "moduleweb/builder/attribute_builder.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_module_builder moduleweb_module_builder;

typedef struct moduleweb_field {
    u16 modifiers;
    OWNED_OBJECT char* name;
    OWNED_OBJECT char* descriptor;

    moduleweb_attribute_vector attributes;
} moduleweb_field;

typedef struct moduleweb_field_builder {
    UNOWNED_OBJECT moduleweb_module_builder* module;

    u16 modifiers;
    OWNED_OBJECT char* name;
    OWNED_OBJECT char* descriptor;

    moduleweb_attribute_vector attributes;
} moduleweb_field_builder;

void moduleweb_field_delete(moduleweb_field* field);

/**
 * Will free all objects owned by the builder (in case it hasn't built)
 */
void moduleweb_field_builder_delete(moduleweb_field_builder* builder);

void moduleweb_field_builder_modifiers(moduleweb_field_builder* builder, u16 modifiers);

void moduleweb_field_builder_descriptor(moduleweb_field_builder* builder, PARAM_COPIED const char* name, PARAM_COPIED const char* descriptor);

void moduleweb_field_builder_add_attribute(moduleweb_field_builder* builder, moduleweb_attribute* attribute);

moduleweb_field* moduleweb_field_builder_build(moduleweb_field_builder* builder);

#ifdef __cplusplus
}
#endif

#endif // JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_FIELD_BUILDER_H
