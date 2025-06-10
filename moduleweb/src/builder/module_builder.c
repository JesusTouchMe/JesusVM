// Copyright 2025 JesusTouchMe

#include "moduleweb/builder/module_builder.h"

void moduleweb_constant_delete(moduleweb_constant* constant) {
    constant->index = 0;
    moduleweb_constant_info_uninit(&constant->info);
    constant->info = (moduleweb_constant_info) {0};
}

void moduleweb_module_builder_delete(moduleweb_module_builder* builder) {
    builder->bytecode_version = 0;

    if (builder->name != NULL) {
        free(builder->name);
        builder->name = NULL;
    }

    moduleweb_attribute_vector_delete(&builder->attributes);

    for (u8 i = 0; i < MODULEWEB_CONSTANT_TYPE_AMOUNT; i++) {
        if (builder->constant_pool[i].pool != NULL) {
            for (u16 j = 0; j < builder->constant_pool[i].size; j++) {
                moduleweb_constant_delete(&builder->constant_pool[i].pool[j]);
            }

            free(builder->constant_pool[i].pool);
            builder->constant_pool[i].pool = NULL;
        }

        builder->constant_pool[i].size = 0;
        builder->constant_pool[i].capacity = 0;
    }

    builder->constant_pool_index = 0;

    if (builder->classes != NULL) {
        for (u16 i = 0; i < builder->class_count; i++) {
            moduleweb_class_delete(&builder->classes[i]);
        }

        free(builder->classes);
        builder->classes = NULL;
    }

    builder->class_count = 0;
    builder->class_capacity = 0;

    if (builder->global_vars != NULL) {
        for (u16 i = 0; i < builder->global_var_count; i++) {
            moduleweb_global_var_delete(&builder->global_vars[i]);
        }

        free(builder->global_vars);
        builder->global_vars = NULL;
    }

    builder->global_var_count = 0;
    builder->global_var_capacity = 0;

    if (builder->functions != NULL) {
        for (u16 i = 0; i < builder->function_count; i++) {
            moduleweb_function_delete(&builder->functions[i]);
        }

        free(builder->functions);
        builder->functions = NULL;
    }

    builder->function_count = 0;
    builder->function_capacity = 0;
}

void moduleweb_module_builder_version(moduleweb_module_builder* builder, u16 version) {
    builder->bytecode_version = version;
}

void moduleweb_module_builder_name(moduleweb_module_builder* builder, PARAM_COPIED const char* name) {
    builder->name = strdup(name);
}

void moduleweb_module_builder_add_attribute(moduleweb_module_builder* builder, PARAM_MOVED moduleweb_attribute* attribute) {
    moduleweb_attribute_vector_add(&builder->attributes, attribute);
    memset(attribute, 0, sizeof(moduleweb_attribute)); // specified PARAM_MOVED, so here we simply zero-set after copying
}

void moduleweb_module_builder_add_class(moduleweb_module_builder* builder, PARAM_MOVED moduleweb_class* clas) {
    VECTOR_ADD(builder->class_count, builder->class_capacity, builder->classes, *clas, moduleweb_class);
    memset(clas, 0, sizeof(moduleweb_class));
}

void moduleweb_module_builder_add_global_var(moduleweb_module_builder* builder, PARAM_MOVED moduleweb_global_var* global_var) {
    VECTOR_ADD(builder->global_var_count, builder->global_var_capacity, builder->global_vars, *global_var, moduleweb_global_var);
    memset(global_var, 0, sizeof(moduleweb_global_var));
}

void moduleweb_module_builder_add_function(moduleweb_module_builder* builder, PARAM_MOVED moduleweb_function* function) {
    VECTOR_ADD(builder->function_count, builder->function_capacity, builder->functions, *function, moduleweb_function);
    memset(function, 0, sizeof(moduleweb_function));
}

static void ensure_const_pool_capacity(moduleweb_constant_vector* pool) {
    if (pool->pool == NULL) {
        pool->capacity = 4;
        pool->pool = calloc(pool->capacity, sizeof(moduleweb_constant));
    } else if (pool->size >= pool->capacity) {
        pool->capacity *= 2;
        pool->pool = realloc(pool->pool, pool->capacity * sizeof(moduleweb_constant));
    }
}

#define GET_CONST_POOL(builder, type) (&(builder)->constant_pool[type])

u16 moduleweb_module_builder_resolve_string(moduleweb_module_builder* builder, const char* string) {
    moduleweb_constant_vector* pool = GET_CONST_POOL(builder, MODULEWEB_CONSTANT_TYPE_ASCII);
    u64 string_length = strlen(string);

    if (pool->pool != NULL) {
        for (u16 i = 0; i < pool->size; i++) {
            if (pool->pool[i].info.ascii_info.length == string_length
                && memcmp(pool->pool[i].info.ascii_info.bytes, string, string_length) == 0) {
                return pool->pool[i].index;
            }
        }
    }

    ensure_const_pool_capacity(pool);

    moduleweb_constant* ascii = &pool->pool[pool->size++];
    ascii->index = builder->constant_pool_index++;

    ascii->info.type = MODULEWEB_CONSTANT_TYPE_ASCII;
    ascii->info.ascii_info.length = string_length;
    ascii->info.ascii_info.bytes = malloc(string_length);
    memcpy(ascii->info.ascii_info.bytes, string, string_length);

    return ascii->index;
}

u16 moduleweb_module_builder_resolve_name(moduleweb_module_builder* builder, const char* name, const char* descriptor) {
    moduleweb_constant_vector* pool = GET_CONST_POOL(builder, MODULEWEB_CONSTANT_TYPE_NAME);

    u16 name_index = moduleweb_module_builder_resolve_string(builder, name);
    u16 descriptor_index = moduleweb_module_builder_resolve_string(builder, descriptor);

    if (pool->pool != NULL) {
        for (u16 i = 0; i < pool->size; i++) {
            if (pool->pool[i].info.name_info.name_index == name_index
                && pool->pool[i].info.name_info.descriptor_index == descriptor_index) {
                return pool->pool[i].index;
            }
        }
    }

    ensure_const_pool_capacity(pool);

    moduleweb_constant* name_entry = &pool->pool[pool->size++];
    name_entry->index = builder->constant_pool_index++;

    name_entry->info.type = MODULEWEB_CONSTANT_TYPE_NAME;
    name_entry->info.name_info.name_index = name_index;
    name_entry->info.name_info.descriptor_index = descriptor_index;

    return name_entry->index;
}

u16 moduleweb_module_builder_resolve_module_ref(moduleweb_module_builder* builder, const char* name) {
    moduleweb_constant_vector* pool = GET_CONST_POOL(builder, MODULEWEB_CONSTANT_TYPE_MODULE_REF);

    u16 name_index = moduleweb_module_builder_resolve_string(builder, name);

    if (pool->pool != NULL) {
        for (u16 i = 0; i < pool->size; i++) {
            if (pool->pool[i].info.module_ref_info.name_index == name_index) {
                return pool->pool[i].index;
            }
        }
    }

    ensure_const_pool_capacity(pool);

    moduleweb_constant* module = &pool->pool[pool->size++];
    module->index = builder->constant_pool_index++;

    module->info.type = MODULEWEB_CONSTANT_TYPE_MODULE_REF;
    module->info.module_ref_info.name_index = name_index;

    return module->index;
}

u16 moduleweb_module_builder_resolve_global_var_ref(moduleweb_module_builder* builder, const char* module_name, const char* name, const char* descriptor) {
    moduleweb_constant_vector* pool = GET_CONST_POOL(builder, MODULEWEB_CONSTANT_TYPE_GLOBAL_VAR_REF);

    u16 module_index = moduleweb_module_builder_resolve_module_ref(builder, module_name);
    u16 name_index = moduleweb_module_builder_resolve_name(builder, name, descriptor);

    if (pool->pool != NULL) {
        for (u16 i = 0; i < pool->size; i++) {
            if (pool->pool[i].info.global_var_ref_info.module_index == module_index
                && pool->pool[i].info.global_var_ref_info.name_info_index == name_index) {
                return pool->pool[i].index;
            }
        }
    }

    ensure_const_pool_capacity(pool);

    moduleweb_constant* global_var = &pool->pool[pool->size++];
    global_var->index = builder->constant_pool_index++;

    global_var->info.type = MODULEWEB_CONSTANT_TYPE_GLOBAL_VAR_REF;
    global_var->info.global_var_ref_info.module_index = module_index;
    global_var->info.global_var_ref_info.name_info_index = name_index;

    return global_var->index;
}

u16 moduleweb_module_builder_resolve_function_ref(moduleweb_module_builder* builder, const char* module_name, const char* name, const char* descriptor) {
    moduleweb_constant_vector* pool = GET_CONST_POOL(builder, MODULEWEB_CONSTANT_TYPE_FUNCTION_REF);

    u16 module_index = moduleweb_module_builder_resolve_module_ref(builder, module_name);
    u16 name_index = moduleweb_module_builder_resolve_name(builder, name, descriptor);

    if (pool->pool != NULL) {
        for (u16 i = 0; i < pool->size; i++) {
            if (pool->pool[i].info.function_ref_info.module_index == module_index
                && pool->pool[i].info.function_ref_info.name_info_index == name_index) {
                return pool->pool[i].index;
            }
        }
    }

    ensure_const_pool_capacity(pool);

    moduleweb_constant* function = &pool->pool[pool->size++];
    function->index = builder->constant_pool_index++;

    function->info.type = MODULEWEB_CONSTANT_TYPE_FUNCTION_REF;
    function->info.function_ref_info.module_index = module_index;
    function->info.function_ref_info.name_info_index = name_index;

    return function->index;
}

u16 moduleweb_module_builder_resolve_class_ref(moduleweb_module_builder* builder, const char* module_name, const char* name) {
    moduleweb_constant_vector* pool = GET_CONST_POOL(builder, MODULEWEB_CONSTANT_TYPE_CLASS_REF);

    u16 module_index = moduleweb_module_builder_resolve_module_ref(builder, module_name);
    u16 name_index = moduleweb_module_builder_resolve_string(builder, name);

    if (pool->pool != NULL) {
        for (u16 i = 0; i < pool->size; i++) {
            if (pool->pool[i].info.class_ref_info.module_index == module_index && pool->pool[i].info.class_ref_info.name_index == name_index) {
                return pool->pool[i].index;
            }
        }
    }

    ensure_const_pool_capacity(pool);

    moduleweb_constant* class = &pool->pool[pool->size++];
    class->index = builder->constant_pool_index++;

    class->info.type = MODULEWEB_CONSTANT_TYPE_CLASS_REF;
    class->info.class_ref_info.module_index = module_index;
    class->info.class_ref_info.name_index = name_index;

    return class->index;
}

u16 moduleweb_module_builder_resolve_field_ref(moduleweb_module_builder* builder, const char* owner_module, const char* owner,
                                               const char* name, const char* descriptor) {
    moduleweb_constant_vector* pool = GET_CONST_POOL(builder, MODULEWEB_CONSTANT_TYPE_FIELD_REF);

    u16 owner_index = moduleweb_module_builder_resolve_class_ref(builder, owner_module, owner);
    u16 name_index = moduleweb_module_builder_resolve_name(builder, name, descriptor);

    if (pool->pool != NULL) {
        for (u16 i = 0; i < pool->size; i++) {
            if (pool->pool[i].info.field_ref_info.class_index == owner_index && pool->pool[i].info.field_ref_info.name_info_index == name_index) {
                return pool->pool[i].index;
            }
        }
    }

    ensure_const_pool_capacity(pool);

    moduleweb_constant* field = &pool->pool[pool->size++];
    field->index = builder->constant_pool_index++;

    field->info.type = MODULEWEB_CONSTANT_TYPE_FIELD_REF;
    field->info.field_ref_info.class_index = owner_index;
    field->info.field_ref_info.name_info_index = name_index;

    return field->index;
}

u16 moduleweb_module_builder_resolve_method_ref(moduleweb_module_builder* builder, const char* owner_module, const char* owner,
                                               const char* name, const char* descriptor) {
    moduleweb_constant_vector* pool = GET_CONST_POOL(builder, MODULEWEB_CONSTANT_TYPE_METHOD_REF);

    u16 owner_index = moduleweb_module_builder_resolve_class_ref(builder, owner_module, owner);
    u16 name_index = moduleweb_module_builder_resolve_name(builder, name, descriptor);

    if (pool->pool != NULL) {
        for (u16 i = 0; i < pool->size; i++) {
            if (pool->pool[i].info.method_ref_info.class_index == owner_index && pool->pool[i].info.method_ref_info.name_info_index == name_index) {
                return pool->pool[i].index;
            }
        }
    }

    ensure_const_pool_capacity(pool);

    moduleweb_constant* method = &pool->pool[pool->size++];
    method->index = builder->constant_pool_index++;

    method->info.type = MODULEWEB_CONSTANT_TYPE_METHOD_REF;
    method->info.field_ref_info.class_index = owner_index;
    method->info.field_ref_info.name_info_index = name_index;

    return method->index;
}

static void moduleweb_module_builder_transfer_attributes(moduleweb_module_builder* builder,
                                                         moduleweb_attribute_vector* src,
                                                         moduleweb_attribute_array* dest) {
    dest->size = src->size;
    dest->array = malloc(dest->size * sizeof(moduleweb_attribute_info));

    for (u16 i = 0; i < dest->size; i++) {
        dest->array[i].name_index = moduleweb_module_builder_resolve_string(builder, src->data[i].name);
        dest->array[i].length = src->data[i].length;
        dest->array[i].info = src->data[i].info;

        src->data[i].info = NULL;
    }

    moduleweb_attribute_vector_delete(src);
}

void moduleweb_module_builder_build(moduleweb_module_builder* builder, PARAM_MUTATED moduleweb_module_info* info) {
    info->magic = MODULEWEB_MAGIC_NUMBER;

    MOVE(info->bytecode_version, builder->bytecode_version);
    MOVE(info->name, builder->name);

    moduleweb_module_builder_transfer_attributes(builder, &builder->attributes, &info->attributes);

    info->class_count = builder->class_count;
    info->classes = malloc(info->class_count * sizeof(moduleweb_class_info));

    for (u16 i = 0; i < info->class_count; i++) {
        moduleweb_class* src = &builder->classes[i];
        moduleweb_class_info* dest = &info->classes[i];

        dest->name_index = moduleweb_module_builder_resolve_string(builder, src->name);
        dest->modifiers = src->modifiers;
        if (src->super_class_module_name != NULL && src->super_class_name != NULL) {
            dest->super_class = moduleweb_module_builder_resolve_class_ref(builder, src->super_class_module_name,src->super_class_name);
        } else {
            dest->super_class = 0;
        }

        moduleweb_module_builder_transfer_attributes(builder, &src->attributes, &dest->attributes);

        dest->field_count = src->field_count;
        dest->fields = malloc(dest->field_count * sizeof(moduleweb_class_info));

        for (u16 j = 0; j < dest->field_count; j++) {
            moduleweb_field* src_field = &src->fields[j];
            moduleweb_field_info* dest_field = &dest->fields[j];

            dest_field->modifiers = src_field->modifiers;
            dest_field->name_index = moduleweb_module_builder_resolve_name(builder, src_field->name, src_field->descriptor);
            moduleweb_module_builder_transfer_attributes(builder, &src_field->attributes, &dest_field->attributes);

            moduleweb_field_delete(src_field);
        }

        dest->method_count = src->method_count;
        dest->methods = malloc(dest->method_count * sizeof(moduleweb_method_info));

        for (u16 j = 0; j < dest->method_count; j++) {
            moduleweb_method* src_method = &src->methods[j];
            moduleweb_method_info* dest_method = &dest->methods[j];

            dest_method->modifiers = src_method->modifiers;
            dest_method->name_index = moduleweb_module_builder_resolve_name(builder, src_method->name, src_method->descriptor);
            dest_method->function_index = moduleweb_module_builder_resolve_function_ref(builder, info->name, src_method->function->name, src_method->function->descriptor);
            moduleweb_module_builder_transfer_attributes(builder, &src_method->attributes, &dest_method->attributes);

            moduleweb_method_delete(src_method);
        }

        moduleweb_class_delete(src);
    }

    info->global_var_count = builder->global_var_count;
    info->global_vars = malloc(info->global_var_count * sizeof(moduleweb_global_var_info));

    for (u16 i = 0; i < info->global_var_count; i++) {
        moduleweb_global_var* src = &builder->global_vars[i];
        moduleweb_global_var_info* dest = &info->global_vars[i];

        dest->modifiers = src->modifiers;
        dest->name_index = moduleweb_module_builder_resolve_name(builder, src->name, src->descriptor);
        moduleweb_module_builder_transfer_attributes(builder, &src->attributes, &dest->attributes);

        moduleweb_global_var_delete(src);
    }

    info->function_count = builder->function_count;
    info->functions = malloc(info->function_count * sizeof(moduleweb_function_info));

    for (u16 i = 0; i < info->function_count; i++) {
        moduleweb_function* src = &builder->functions[i];
        moduleweb_function_info* dest = &info->functions[i];

        dest->modifiers = src->modifiers;
        dest->name_index = moduleweb_module_builder_resolve_name(builder, src->name, src->descriptor);
        moduleweb_module_builder_transfer_attributes(builder, &src->attributes, &dest->attributes);

        moduleweb_function_delete(src);
    }

    u32 total_constant_pool_size = 1;
    for (u8 i = 0; i < (u8) MODULEWEB_CONSTANT_TYPE_AMOUNT; i++) {
        total_constant_pool_size += builder->constant_pool[i].size;
    }

    if (total_constant_pool_size > (u16) 0xFFFF) {
        printf("warning: constant pool size of module '%s' is bigger than the maximum possible size (%u > %u)",
               info->name, total_constant_pool_size, (u16) 0xFFFF);

        return;
    }

    info->constant_pool_size = total_constant_pool_size;
    info->constant_pool = malloc(info->constant_pool_size * sizeof(moduleweb_constant_info));

    for (moduleweb_constant_type i = 0; i < (u8) MODULEWEB_CONSTANT_TYPE_AMOUNT; i++) {
        for (u16 j = 0; j < builder->constant_pool[i].size; j++) {
            moduleweb_constant* src = &builder->constant_pool[i].pool[j];

            info->constant_pool[src->index] = src->info;
            info->constant_pool[src->index].type = i;

            *src = (moduleweb_constant) {0};

            moduleweb_constant_delete(src);
        }
    }

    moduleweb_module_builder_delete(builder);
}
