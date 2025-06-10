// Copyright 2025 JesusTouchMe

#include "moduleweb/builder/method_builder.h"

void moduleweb_method_delete(moduleweb_method* method) {
    method->modifiers = 0;

    if (method->name != NULL) {
        free(method->name);
        method->name = NULL;
    }

    if (method->descriptor != NULL) {
        free(method->descriptor);
        method->descriptor = NULL;
    }

    moduleweb_attribute_vector_delete(&method->attributes);
}

void moduleweb_method_builder_delete(moduleweb_method_builder* builder) {
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

void moduleweb_method_builder_modifiers(moduleweb_method_builder* builder, u16 modifiers) {
    builder->modifiers = modifiers;
}

void moduleweb_method_builder_descriptor(moduleweb_method_builder* builder, const char* name, const char* descriptor) {
    builder->name = strdup(name);
    builder->descriptor = strdup(descriptor);
}

void moduleweb_method_builder_function(moduleweb_method_builder* builder, moduleweb_function* function) {
    builder->function = function;
}

void moduleweb_method_builder_add_attribute(moduleweb_method_builder* builder, moduleweb_attribute* attribute) {
    moduleweb_attribute_vector_add(&builder->attributes, attribute);
    memset(attribute, 0, sizeof(moduleweb_attribute)); // specified PARAM_MOVED, so here we simply zero-set after copying
}

moduleweb_method* moduleweb_method_builder_build(moduleweb_method_builder* builder) {
    static moduleweb_method result; // TODO: better solution

    MOVE(result.modifiers, builder->modifiers);
    MOVE(result.name, builder->name);
    MOVE(result.descriptor, builder->descriptor);
    MOVE(result.function, builder->function);
    MOVE_S(result.attributes, builder->attributes, moduleweb_attribute_vector);

    return &result;
}
