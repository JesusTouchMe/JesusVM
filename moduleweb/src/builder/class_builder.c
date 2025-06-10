// Copyright 2025 JesusTouchMe

#include "moduleweb/builder/class_builder.h"

void moduleweb_class_delete(moduleweb_class* clas) {
    if (clas->name != NULL) {
        free(clas->name);
        clas->name = NULL;
    }

    clas->modifiers = 0;

    if (clas->super_class_module_name != NULL) {
        free(clas->super_class_module_name);
        clas->super_class_module_name = NULL;
    }

    if (clas->super_class_name != NULL) {
        free(clas->super_class_name);
        clas->super_class_name = NULL;
    }

    moduleweb_attribute_vector_delete(&clas->attributes);

    if (clas->fields != NULL) {
        for (u16 i = 0; i < clas->field_count; i++) {
            moduleweb_field_delete(&clas->fields[i]);
        }

        free(clas->fields);
        clas->fields = NULL;
    }

    clas->field_count = 0;
}

void moduleweb_class_builder_delete(moduleweb_class_builder* builder) {
    if (builder->name != NULL) {
        free(builder->name);
        builder->name = NULL;
    }

    builder->modifiers = 0;

    if (builder->super_class_module_name != NULL) {
        free(builder->super_class_module_name);
        builder->super_class_module_name = NULL;
    }

    if (builder->super_class_name != NULL) {
        free(builder->super_class_name);
        builder->super_class_name = NULL;
    }

    moduleweb_attribute_vector_delete(&builder->attributes);

    if (builder->fields != NULL) {
        for (u16 i = 0; i < builder->field_count; i++) {
            moduleweb_field_delete(&builder->fields[i]);
        }

        free(builder->fields);
        builder->fields = NULL;
    }

    builder->field_count = 0;
    builder->field_capacity = 0;
}

void moduleweb_class_builder_name(moduleweb_class_builder* builder, PARAM_COPIED const char* name) {
    builder->name = strdup(name);
}

void moduleweb_class_builder_modifiers(moduleweb_class_builder* builder, u16 modifiers) {
    builder->modifiers = modifiers;
}

void moduleweb_class_builder_super_class(moduleweb_class_builder* builder, PARAM_COPIED const char* super_module, PARAM_COPIED const char* super_class) {
    builder->super_class_module_name = strdup(super_module);
    builder->super_class_name = strdup(super_class);
}

void moduleweb_class_builder_add_attribute(moduleweb_class_builder* builder, PARAM_MOVED moduleweb_attribute* attribute) {
    moduleweb_attribute_vector_add(&builder->attributes, attribute);
    memset(attribute, 0, sizeof(moduleweb_attribute)); // specified PARAM_MOVED, so here we simply zero-set after copying
}

void moduleweb_class_builder_add_field(moduleweb_class_builder* builder, PARAM_MOVED moduleweb_field* field) {
    VECTOR_ADD(builder->field_count, builder->field_capacity, builder->fields, *field, moduleweb_field);
    memset(field, 0, sizeof(moduleweb_field));
}

void moduleweb_class_builder_add_method(moduleweb_class_builder* builder, PARAM_MOVED moduleweb_method* method) {
    VECTOR_ADD(builder->method_count, builder->method_capacity, builder->methods, *method, moduleweb_method);
    memset(method, 0, sizeof(moduleweb_method));
}

moduleweb_class* moduleweb_class_builder_build(moduleweb_class_builder* builder) {
    static moduleweb_class result; // TODO: better solution

    MOVE(result.name, builder->name);
    MOVE(result.modifiers, builder->modifiers);
    MOVE(result.super_class_module_name, builder->super_class_module_name);
    MOVE(result.super_class_name, builder->super_class_name);
    MOVE_S(result.attributes, builder->attributes, moduleweb_attribute_vector);
    MOVE(result.field_count, builder->field_count);
    MOVE(result.fields, builder->fields);
    MOVE(result.method_count, builder->method_count);
    MOVE(result.methods, builder->methods);

    return &result;
}