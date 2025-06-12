// Copyright 2025 JesusTouchMe

#include "moduleweb/constant_info.h"

#include "moduleweb/builder/insn_builder.h"
#include "moduleweb/builder/module_builder.h"
#include "moduleweb/builder/opcodes.h"

int moduleweb_insn_list_init(moduleweb_insn_list* list) {
    memset(list, 0, sizeof(moduleweb_insn_list));

    list->size = 64;
    list->buffer = malloc(list->size);

    if (list->buffer == NULL) {
        return 1;
    }

    if (moduleweb_outstream_init_buffer(&list->writer_stream, list->buffer, list->size)) {
        return 1;
    }

    return 0;
}

void moduleweb_insn_list_uninit(moduleweb_insn_list* list) {
    free(list->buffer);
    list->buffer = NULL;
    list->size = 0;

    free(list->forward_labels);
    list->forward_labels = NULL;

    for (u64 i = 0; i < list->label_count; i++) {
        free(list->labels[i]->name);
        free(list->labels[i]);
    }

    free(list->labels);
    list->labels = NULL;
    list->label_count = 0;

    moduleweb_outstream_uninit_buffer(&list->writer_stream);
}

void moduleweb_insn_list_patch_labels(moduleweb_insn_list* list) {
    for (u64 i = 0; i < list->forward_label_count; i++) {
        moduleweb_forward_label* label = &list->forward_labels[i];

        u64 symbol = label->label->location - label->origin;

        switch (label->size) {
            case 1:
                list->buffer[label->location]       = (u8) (symbol & 0xFF);
                break;

            case 2:
                list->buffer[label->location]       = (u8) ((symbol >> 8) & 0xFF);
                list->buffer[label->location + 1]   = (u8) (symbol & 0xFF);
                break;

            case 4:
                list->buffer[label->location]       = (u8) ((symbol >> 24) & 0xFF);
                list->buffer[label->location + 1]   = (u8) ((symbol >> 16) & 0xFF);
                list->buffer[label->location + 2]   = (u8) ((symbol >> 8) & 0xFF);
                list->buffer[label->location + 3]   = (u8) (symbol & 0xFF);
                break;

            case 8:
                list->buffer[label->location]       = (u8) ((symbol >> 56) & 0xFF);
                list->buffer[label->location + 1]   = (u8) ((symbol >> 48) & 0xFF);
                list->buffer[label->location + 2]   = (u8) ((symbol >> 40) & 0xFF);
                list->buffer[label->location + 3]   = (u8) ((symbol >> 32) & 0xFF);
                list->buffer[label->location + 4]   = (u8) ((symbol >> 24) & 0xFF);
                list->buffer[label->location + 5]   = (u8) ((symbol >> 16) & 0xFF);
                list->buffer[label->location + 6]   = (u8) ((symbol >> 8) & 0xFF);
                list->buffer[label->location + 7]   = (u8) (symbol & 0xFF);
                break;

            default:
                printf("warning: unsupported patch size %u for label: %s\n", label->size, label->label->name);
                break;
        }
    }
}

moduleweb_label* moduleweb_insn_list_get_label(const moduleweb_insn_list* list, const char* name) {
    if (name == NULL) return NULL;

    for (u64 i = 0; i < list->label_count; i++) {
        if (strcmp(list->labels[i]->name, name) == 0) {
            return list->labels[i];
        }
    }

    return NULL;
}

static int moduleweb_insn_list_prepare(moduleweb_insn_list* list, u64 size) {
    if (list->writer_stream.memory.pos + size >= list->size) {
        list->size *= 2;
        u8* new_buffer = realloc(list->buffer, list->size);

        if (new_buffer == NULL) {
            return 1;
        }

        list->buffer = new_buffer;

        if (moduleweb_outstream_reopen_buffer(&list->writer_stream, list->buffer, list->size)) {
            return 1;
        }
    }

    return 0;
}

void moduleweb_insn_list_insn(moduleweb_insn_list* list, u8 opcode) {
    if (moduleweb_insn_list_prepare(list, 1)) {
        return;
    }

    moduleweb_outstream_write_u8(&list->writer_stream, opcode);
}

void moduleweb_insn_list_global_var(moduleweb_insn_list* list, u8 opcode, const char* module, const char* name, const char* descriptor) {
    if (moduleweb_insn_list_prepare(list, 3)) {
        return;
    }

    if (moduleweb_outstream_write_u8(&list->writer_stream, opcode)) {
        return;
    }

    u16 index = moduleweb_module_builder_resolve_global_var_ref(list->module, module, name, descriptor);

    if (moduleweb_outstream_write_u16(&list->writer_stream, index)) {
        return;
    }
}

void moduleweb_insn_list_call(moduleweb_insn_list* list, u8 opcode, const char* module, const char* name, const char* descriptor) {
    if (moduleweb_insn_list_prepare(list, 3)) {
        return;
    }

    if (moduleweb_outstream_write_u8(&list->writer_stream, opcode)) {
        return;
    }

    u16 index = moduleweb_module_builder_resolve_function_ref(list->module, module, name, descriptor);

    if (moduleweb_outstream_write_u16(&list->writer_stream, index)) {
        return;
    }
}

moduleweb_label* moduleweb_insn_list_create_label(moduleweb_insn_list* list, NULLABLE const char* name) {
    moduleweb_label* label = malloc(sizeof(moduleweb_label));
    if (label == NULL) {
        return NULL;
    }

    label->location_init = false;

    if (name == NULL) {
        u64 length = snprintf(NULL, 0, ".%llu", list->label_count) + 1;

        char* new_name = malloc(length);
        if (new_name == NULL) {
            free(label);
            return NULL;
        }

        snprintf(new_name, length, ".%llu", list->label_count);

        label->name = new_name;
    } else {
        label->name = strdup(name);
    }

    return label;
}

void moduleweb_insn_list_add_label(moduleweb_insn_list* list, moduleweb_label* label) {
    label->location = list->writer_stream.memory.pos;
    label->location_init = true;

    VECTOR_ADD(list->label_count, list->label_capacity, list->labels, label, moduleweb_label*);
}

static void moduleweb_insn_list_forward_label(moduleweb_insn_list* list, moduleweb_label* label, u8 size, i32 offset) {
    moduleweb_forward_label forward;

    forward.label = label;
    forward.size = size;
    forward.location = list->writer_stream.memory.pos + offset;
    forward.origin = list->writer_stream.memory.pos;

    VECTOR_ADD(list->forward_label_count, list->forward_label_capacity, list->forward_labels, forward, moduleweb_forward_label);
}

void moduleweb_insn_list_jump(moduleweb_insn_list* list, u8 opcode, moduleweb_label* label) {
    if (moduleweb_insn_list_prepare(list, 3)) {
        return;
    }

    if (moduleweb_outstream_write_u8(&list->writer_stream, opcode)) {
        return;
    }

    if (moduleweb_outstream_write_u16(&list->writer_stream, label->location - list->writer_stream.memory.pos - 2)) {
        return;
    }

    if (!label->location_init) {
        moduleweb_insn_list_forward_label(list, label, 2, -2);
    }
}

void moduleweb_insn_list_class(moduleweb_insn_list* list, u8 opcode, const char* module, const char* name) {
    if (moduleweb_insn_list_prepare(list, 3)) {
        return;
    }

    if (moduleweb_outstream_write_u8(&list->writer_stream, opcode)) {
        return;
    }

    if (moduleweb_outstream_write_u16(&list->writer_stream,
                                      moduleweb_module_builder_resolve_class_ref(list->module, module, name))) {
        return;
    }
}

void moduleweb_insn_list_field(moduleweb_insn_list* list, u8 opcode, const char* owner_module, const char* owner, const char* name, const char* descriptor) {
    if (moduleweb_insn_list_prepare(list, 3)) {
        return;
    }

    if (moduleweb_outstream_write_u8(&list->writer_stream, opcode)) {
        return;
    }

    if (moduleweb_outstream_write_u16(&list->writer_stream,
    moduleweb_module_builder_resolve_field_ref(list->module, owner_module, owner, name, descriptor))) {
        return;
    }
}

void moduleweb_insn_list_method(moduleweb_insn_list* list, u8 opcode, const char* owner_module, const char* owner, const char* name, const char* descriptor) {
    if (moduleweb_insn_list_prepare(list, 3)) {
        return;
    }

    if (moduleweb_outstream_write_u8(&list->writer_stream, opcode)) {
        return;
    }

    if (moduleweb_outstream_write_u16(&list->writer_stream,
        moduleweb_module_builder_resolve_method_ref(list->module, owner_module, owner, name, descriptor))) {
        return;
    }
}

void moduleweb_insn_list_ldc_str(moduleweb_insn_list* list, const char* value) {
    if (moduleweb_insn_list_prepare(list, 3)) {
        return;
    }

    if (moduleweb_outstream_write_u8(&list->writer_stream, LDC)) {
        return;
    }

    if (moduleweb_outstream_write_u16(&list->writer_stream, moduleweb_module_builder_resolve_string(list->module, value))) {
        return;
    }
}

void moduleweb_insn_list_var(moduleweb_insn_list* list, u8 opcode, u16 local) {
    if (local > 0xFF) {
        if (moduleweb_insn_list_prepare(list, 4)) {
            return;
        }

        if (moduleweb_outstream_write_u8(&list->writer_stream, list->wide_opcode)) {
            return;
        }

        if (moduleweb_outstream_write_u8(&list->writer_stream, opcode)) {
            return;
        }

        if (moduleweb_outstream_write_u16(&list->writer_stream, local)) {
            return;
        }
    } else {
        if (moduleweb_insn_list_prepare(list, 2)) {
            return;
        }

        if (moduleweb_outstream_write_u8(&list->writer_stream, opcode)) {
            return;
        }

        if (moduleweb_outstream_write_u8(&list->writer_stream, local)) {
            return;
        }
    }
}

void moduleweb_insn_list_inc(moduleweb_insn_list* list, u8 opcode, u16 local, i16 increment) {
    bool wide = false;
    if (local > 0xFF || (increment < -128 || increment > 127)) {
        wide = true;
        if (moduleweb_insn_list_prepare(list, 5)) {
            return;
        }
    } else {
        if (moduleweb_insn_list_prepare(list, 3)) {
            return;
        }
    }

    if (wide) {
        if (moduleweb_outstream_write_u8(&list->writer_stream, WIDE)) {
            return;
        }
    }

    if (moduleweb_outstream_write_u8(&list->writer_stream, opcode)) {
        return;
    }

    if (wide) {
        if (moduleweb_outstream_write_u16(&list->writer_stream, local)) {
            return;
        }

        if (moduleweb_outstream_write_u16(&list->writer_stream, increment)) {
            return;
        }
    } else {
        if (moduleweb_outstream_write_u8(&list->writer_stream, local)) {
            return;
        }

        if (moduleweb_outstream_write_u8(&list->writer_stream, increment)) {
            return;
        }
    }
}

void moduleweb_insn_list_byte(moduleweb_insn_list* list, u8 opcode, i8 operand) {
    if (moduleweb_insn_list_prepare(list, 2)) {
        return;
    }

    if (moduleweb_outstream_write_u8(&list->writer_stream, opcode)) {
        return;
    }

    if (moduleweb_outstream_write_u8(&list->writer_stream, (u8) operand)) {
        return;
    }
}

void moduleweb_insn_list_short(moduleweb_insn_list* list, u8 opcode, i16 operand) {
    if (moduleweb_insn_list_prepare(list, 3)) {
        return;
    }

    if (moduleweb_outstream_write_u8(&list->writer_stream, opcode)) {
        return;
    }

    if (moduleweb_outstream_write_u16(&list->writer_stream, (u16) operand)) {
        return;
    }
}

void moduleweb_insn_list_int(moduleweb_insn_list* list, u8 opcode, i32 operand) {
    if (moduleweb_insn_list_prepare(list, 5)) {
        return;
    }

    if (moduleweb_outstream_write_u8(&list->writer_stream, opcode)) {
        return;
    }

    if (moduleweb_outstream_write_u32(&list->writer_stream, (u32) operand)) {
        return;
    }
}

void moduleweb_insn_list_long(moduleweb_insn_list* list, u8 opcode, i64 operand) {
    if (moduleweb_insn_list_prepare(list, 9)) {
        return;
    }

    if (moduleweb_outstream_write_u8(&list->writer_stream, opcode)) {
        return;
    }

    if (moduleweb_outstream_write_u64(&list->writer_stream, (u64) operand)) {
        return;
    }
}
