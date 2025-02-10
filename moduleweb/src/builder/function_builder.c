// Copyright 2025 JesusTouchMe

#include "moduleweb/builder/function_builder.h"

void moduleweb_function_delete(moduleweb_function* function) {
    function->modifiers = 0;

    if (function->name != NULL) {
        free(function->name);
        function->name = NULL;
    }

    if (function->descriptor != NULL) {
        free(function->descriptor);
        function->descriptor = NULL;
    }

    moduleweb_attribute_vector_delete(&function->attributes);
}

void moduleweb_function_builder_delete(moduleweb_function_builder* builder) {
    builder->module = NULL;
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

void moduleweb_function_builder_modifiers(moduleweb_function_builder* builder, u16 modifiers) {
    builder->modifiers = modifiers;
}

void moduleweb_function_builder_descriptor(moduleweb_function_builder* builder, PARAM_COPIED const char* name, PARAM_COPIED const char* descriptor) {
    builder->name = strdup(name);
    builder->descriptor = strdup(descriptor);
}

void moduleweb_function_builder_add_attribute(moduleweb_function_builder* builder, moduleweb_attribute* attribute) {
    moduleweb_attribute_vector_add(&builder->attributes, attribute);
    memset(attribute, 0, sizeof(moduleweb_attribute)); // specified PARAM_MOVED, so here we simply zero-set after copying
}

moduleweb_function* moduleweb_function_builder_build(moduleweb_function_builder* builder) {
    static moduleweb_function result; // TODO: better solution

    MOVE(result.modifiers, builder->modifiers);
    MOVE(result.name, builder->name);
    MOVE(result.descriptor, builder->descriptor);
    MOVE_S(result.attributes, builder->attributes, moduleweb_attribute_vector);

    return &result;
}