// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_ATTRIBUTE_BUILDER_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_ATTRIBUTE_BUILDER_H 1

#include "moduleweb/module_info.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_module_builder moduleweb_module_builder;

typedef struct moduleweb_attribute {
    OWNED_OBJECT char* name;
    u32 length;
    OWNED_OBJECT u8* info;
} moduleweb_attribute;

typedef struct moduleweb_attribute_vector {
    u32 size;
    u32 capacity;
    OWNED_OBJECT moduleweb_attribute* data;
} moduleweb_attribute_vector;

typedef struct moduleweb_attribute_builder {
    UNOWNED_OBJECT moduleweb_module_builder* module;

    OWNED_OBJECT char* name;
    u32 length;
    OWNED_OBJECT u8* info;
} moduleweb_attribute_builder;

void moduleweb_attribute_delete(moduleweb_attribute* attribute);

void moduleweb_attribute_vector_add(moduleweb_attribute_vector* vector, PARAM_COPIED moduleweb_attribute* attribute);

void moduleweb_attribute_vector_delete(moduleweb_attribute_vector* vector);

/**
 * Will delete all objects owned by the builder (in case it hasn't built)
 */
void moduleweb_attribute_builder_delete(moduleweb_attribute_builder* builder);

void moduleweb_attribute_builder_name(moduleweb_attribute_builder* builder, PARAM_COPIED const char* name);

void moduleweb_attribute_builder_info(moduleweb_attribute_builder* builder, PARAM_COPIED const void* info, u32 length);

moduleweb_attribute* moduleweb_attribute_builder_build(moduleweb_attribute_builder* builder);

#ifdef __cplusplus
}
#endif

#endif // JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_ATTRIBUTE_BUILDER_H
