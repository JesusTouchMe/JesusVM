// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_CLASS_BUILDER_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_CLASS_BUILDER_H 1

#include "moduleweb/module_info.h"

#include "moduleweb/builder/attribute_builder.h"
#include "moduleweb/builder/field_builder.h"
#include "moduleweb/builder/method_builder.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_module_builder moduleweb_module_builder;

typedef struct moduleweb_class {
    OWNED_OBJECT char* name;
    u16 modifiers;

    OWNED_OBJECT char* super_class_module_name;
    OWNED_OBJECT char* super_class_name;

    moduleweb_attribute_vector attributes;

    u16 field_count;
    OWNED_OBJECT moduleweb_field* fields;

    u16 method_count;
    OWNED_OBJECT moduleweb_method* methods;
} moduleweb_class;

typedef struct moduleweb_class_builder {
    UNOWNED_OBJECT moduleweb_module_builder* module;

    OWNED_OBJECT char* name;
    u16 modifiers;

    OWNED_OBJECT char* super_class_module_name;
    OWNED_OBJECT char* super_class_name;

    moduleweb_attribute_vector attributes;

    u16 field_count;
    u16 field_capacity;
    OWNED_OBJECT moduleweb_field* fields;

    u16 method_count;
    u16 method_capacity;
    OWNED_OBJECT moduleweb_method* methods;
} moduleweb_class_builder;

void moduleweb_class_delete(moduleweb_class* clas);

/**
 * Will free all objects owned by the builder (in case it hasn't built)
 */
void moduleweb_class_builder_delete(moduleweb_class_builder* builder);

void moduleweb_class_builder_name(moduleweb_class_builder* builder, PARAM_COPIED const char* name);

void moduleweb_class_builder_modifiers(moduleweb_class_builder* builder, u16 modifiers);

void moduleweb_class_builder_super_class(moduleweb_class_builder* builder, PARAM_COPIED const char* super_module, PARAM_COPIED const char* super_class);

void moduleweb_class_builder_add_attribute(moduleweb_class_builder* builder, PARAM_MOVED moduleweb_attribute* attribute);

void moduleweb_class_builder_add_field(moduleweb_class_builder* builder, PARAM_MOVED moduleweb_field* field);

void moduleweb_class_builder_add_method(moduleweb_class_builder* builder, PARAM_MOVED moduleweb_method* method);

moduleweb_class* moduleweb_class_builder_build(moduleweb_class_builder* builder);

#ifdef __cplusplus
}
#endif

#endif // JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_CLASS_BUILDER_H
