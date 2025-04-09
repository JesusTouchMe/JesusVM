// Copyright 2025 JesusTouchMe

#include "moduleweb/builder/global_var_builder.h"

void moduleweb_global_var_delete(moduleweb_global_var* global_var) {
    global_var->modifiers = 0;

    if (global_var->name != NULL) {
        free(field->name);
        global_var->name = NULL;
    }

    if (global_var->descriptor != NULL) {
        free(global_var->descriptor);
        global_var->descriptor = NULL;
    }

    moduleweb_attribute_vector_delete(&global_var->attributes);
}

void moduleweb_global_var_builder_delete(moduleweb_global_var_builder* builder) {
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

void moduleweb_global_var_builder_modifiers(moduleweb_global_var_builder* builder, u16 modifiers) {
    builder->modifiers = modifiers;
}

void moduleweb_global_var_builder_descriptor(moduleweb_global_var_builder* builder, PARAM_COPIED const char* name, PARAM_COPIED const char* descriptor) {
    builder->name = strdup(name);
    builder->descriptor = strdup(descriptor);
}

void moduleweb_global_var_builder_add_attribute(moduleweb_global_var_builder* builder, moduleweb_attribute* attribute) {
    moduleweb_attribute_vector_add(&builder->attributes, attribute);
    memset(attribute, 0, sizeof(moduleweb_attribute)); // specified PARAM_MOVED, so here we simply zero-set after copying
}

moduleweb_global_var* moduleweb_global_var_builder_build(moduleweb_global_var_builder* builder) {
    static moduleweb_global_var result;

    MOVE(result.modifiers, builder->modifiers);
    MOVE(result.name, builder->name);
    MOVE(result.descriptor, builder->descriptor);
    MOVE_S(result.attributes, builder->attributes, moduleweb_attribute_vector);

    return &result;
}