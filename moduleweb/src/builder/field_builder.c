// Copyright 2025 JesusTouchMe

#include "moduleweb/builder/field_builder.h"

void moduleweb_field_delete(moduleweb_field* field) {
    field->modifiers = 0;

    if (field->name != NULL) {
        free(field->name);
        field->name = NULL;
    }

    if (field->descriptor != NULL) {
        free(field->descriptor);
        field->descriptor = NULL;
    }

    moduleweb_attribute_vector_delete(&field->attributes);
}

void moduleweb_field_builder_delete(moduleweb_field_builder* builder) {
    builder->modifiers = 0;

    if (builder->name != NULL) {
        free(builder->name);
        builder->name = NULL;
    }

    if (builder->descriptor != NULL) {
        free(builder->descriptor);
        builder->descriptor = NULL;
    }

    moduleweb_attribute_vector_delete(&builder->attributes);
}

void moduleweb_field_builder_modifiers(moduleweb_field_builder* builder, u16 modifiers) {
    builder->modifiers = modifiers;
}

void moduleweb_field_builder_descriptor(moduleweb_field_builder* builder, PARAM_COPIED const char* name, PARAM_COPIED const char* descriptor) {
    builder->name = strdup(name);
    builder->descriptor = strdup(descriptor);
}

void moduleweb_field_builder_add_attribute(moduleweb_field_builder* builder, moduleweb_attribute* attribute) {
    moduleweb_attribute_vector_add(&builder->attributes, attribute);
    memset(attribute, 0, sizeof(moduleweb_attribute)); // specified PARAM_MOVED, so here we simply zero-set after copying
}

moduleweb_field* moduleweb_field_builder_build(moduleweb_field_builder* builder) {
    static moduleweb_field result; // TODO: better solution

    MOVE(result.modifiers, builder->modifiers);
    MOVE(result.name, builder->name);
    MOVE(result.descriptor, builder->descriptor);
    MOVE_S(result.attributes, builder->attributes, moduleweb_attribute_vector);

    return &result;
}