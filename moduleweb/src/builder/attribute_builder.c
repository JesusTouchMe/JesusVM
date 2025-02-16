// Copyright 2025 JesusTouchMe

#include "moduleweb/builder/attribute_builder.h"

void moduleweb_attribute_delete(moduleweb_attribute* attribute) {
    if (attribute->name != NULL) {
        free(attribute->name);
        attribute->name = NULL;
    }

    attribute->length = 0;

    if (attribute->info != NULL) {
        free(attribute->info);
        attribute->info = NULL;
    }
}

void moduleweb_attribute_vector_add(moduleweb_attribute_vector* vector, PARAM_COPIED moduleweb_attribute* attribute) {
    VECTOR_ADD(vector->size, vector->capacity, vector->data, *attribute, moduleweb_attribute);
}

void moduleweb_attribute_vector_delete(moduleweb_attribute_vector* vector) {
    if (vector->data != NULL) {
        for (u32 i = 0; i < vector->size; i++) {
            moduleweb_attribute_delete(&vector->data[i]);
        }

        free(vector->data);
        vector->data = NULL;
    }

    vector->size = 0;
    vector->capacity = 0;
}

void moduleweb_attribute_builder_delete(moduleweb_attribute_builder* builder) {
    if (builder->name != NULL) {
        free(builder->name);
        builder->name = NULL;
    }

    builder->length = 0;

    if (builder->info != NULL) {
        free(builder->info);
        builder->info = NULL;
    }
}

void moduleweb_attribute_builder_name(moduleweb_attribute_builder* builder, PARAM_COPIED const char* name) {
    builder->name = strdup(name);
}

void moduleweb_attribute_builder_info(moduleweb_attribute_builder* builder, PARAM_COPIED const void* info, u32 length) {
    builder->info = malloc(length);

    if (builder->info == NULL) return;

    builder->length = length;
    memcpy(builder->info, info, length);
}

void moduleweb_attribute_builder_code(moduleweb_attribute_builder* builder, PARAM_MOVED moduleweb_insn_list* list) {
    builder->name = strdup("Code");

    moduleweb_insn_list_patch_labels(list);

#ifdef MODULEWEB_OPT_VERIFY_INSN
    if (moduleweb_insn_list_verify(list)) {
        printf("error: verification failed for instruction list\n");
        exit(1);
    }
#endif

    builder->length = list->writer_stream.memory.pos + 8;

    builder->info = malloc(builder->length);
    if (builder->info == NULL) {
        printf("error: memory alloc fail\n");
        return;
    }

    moduleweb_outstream stream;
    if (moduleweb_outstream_init_buffer(&stream, builder->info, builder->length)) {
        free(builder->info);
        printf("error: %s\n", moduleweb_outstream_strerror(&stream));
        return;
    }

    if (moduleweb_outstream_write_u16(&stream, list->local_count)) {
        free(builder->info);
        printf("error: %s\n", moduleweb_outstream_strerror(&stream));
        return;
    }

    if (moduleweb_outstream_write_u16(&stream, list->max_stack_depth)) {
        free(builder->info);
        printf("error: %s\n", moduleweb_outstream_strerror(&stream));
        return;
    }

    if (moduleweb_outstream_write_u32(&stream, list->writer_stream.memory.pos)) {
        free(builder->info);
        printf("error: %s\n", moduleweb_outstream_strerror(&stream));
        return;
    }

    if (moduleweb_outstream_write_bytes(&stream, list->buffer, list->writer_stream.memory.pos)) {
        free(builder->info);
        printf("error: %s\n", moduleweb_outstream_strerror(&stream));
        return;
    }

    moduleweb_outstream_uninit_buffer(&stream);

    moduleweb_insn_list_uninit(list);
}

moduleweb_attribute* moduleweb_attribute_builder_build(moduleweb_attribute_builder* builder) {
    static moduleweb_attribute result = {0}; //TODO: better solution than this lol

    MOVE(result.name, builder->name);
    MOVE(result.length, builder->length);
    MOVE(result.info, builder->info);

    return &result;
}
