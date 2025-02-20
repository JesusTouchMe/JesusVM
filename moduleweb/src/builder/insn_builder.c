// Copyright 2025 JesusTouchMe

#include "moduleweb/constant_info.h"

#include "moduleweb/builder/insn_builder.h"
#include "moduleweb/builder/module_builder.h"
#include "moduleweb/builder/opcodes.h"

u16 moduleweb_module_builder_resolve_string(moduleweb_module_builder* builder, const char* string);
u16 moduleweb_module_builder_resolve_name(moduleweb_module_builder* builder, const char* name, const char* descriptor);
u16 moduleweb_module_builder_resolve_module_ref(moduleweb_module_builder* builder, const char* name);
u16 moduleweb_module_builder_resolve_function_ref(moduleweb_module_builder* builder, const char* module_name, const char* name, const char* descriptor);
u16 moduleweb_module_builder_resolve_class_ref(moduleweb_module_builder* builder, const char* module_name, const char* name);

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

static void moduleweb_insn_list_verify_jump(moduleweb_insn_list* list, i16 branch) {
    if (list->writer_stream.memory.pos > INT64_MAX) {
        printf("error: instruction list contains too many instructions (%llu)\n", list->writer_stream.memory.pos);
        exit(1);
    }

    i64 pc = (i64) list->writer_stream.memory.pos + branch;

    if (pc < 0) {
        printf("error: jump instruction attempts to branch to negative instruction index (%lld)\n", pc);
        exit(1);
    }

    if (pc >= list->writer_stream.memory.pos) {
        printf("error: jump instruction attempts to branch beyond instruction list size (branch: %lld, max: %llu)\n", pc, list->writer_stream.memory.pos - 1);
        exit(1);
    }
}

static void moduleweb_insn_list_verify_stack(moduleweb_insn_list* list) {
    if (list->stack_depth < 0) {
        printf("error: stack underflow in instruction builder.\n (%d < 0)", list->stack_depth);
        exit(1);
    }

    if (list->stack_depth > 0xFFFF) {
        printf("error: stack overflow in instruction builder.\n (%d > %u)", list->stack_depth, 0xFFFF);
        exit(1);
    }
}

static void moduleweb_insn_list_do_push(moduleweb_insn_list* list, i32 value_count) {
    list->stack_depth += value_count;
    moduleweb_insn_list_verify_stack(list);

    list->max_stack_depth = max(list->stack_depth, list->max_stack_depth);
}

static void moduleweb_insn_list_do_pop(moduleweb_insn_list* list, i32 value_count) {
    list->stack_depth -= value_count;
    moduleweb_insn_list_verify_stack(list);

    list->max_stack_depth = max(list->stack_depth, list->max_stack_depth);
}

static moduleweb_constant_ascii_info* moduleweb_insn_list_get_ascii(moduleweb_insn_list* list, u32 index) {
    for (u32 i = 0; i < list->module->constant_pool[MODULEWEB_CONSTANT_TYPE_ASCII].size; i++) {
        if (list->module->constant_pool[MODULEWEB_CONSTANT_TYPE_ASCII].pool[i].index == index) {
            return &list->module->constant_pool[MODULEWEB_CONSTANT_TYPE_ASCII].pool[i].info.ascii_info;
        }
    }

    return NULL;
}

static moduleweb_constant_name_info* moduleweb_insn_list_get_name(moduleweb_insn_list* list, u32 index) {
    for (u32 i = 0; i < list->module->constant_pool[MODULEWEB_CONSTANT_TYPE_NAME].size; i++) {
        if (list->module->constant_pool[MODULEWEB_CONSTANT_TYPE_NAME].pool[i].index == index) {
            return &list->module->constant_pool[MODULEWEB_CONSTANT_TYPE_NAME].pool[i].info.name_info;
        }
    }

    return NULL;
}

static moduleweb_constant_function_ref_info* moduleweb_insn_list_get_function_ref(moduleweb_insn_list* list, u32 index) {
    for (u32 i = 0; i < list->module->constant_pool[MODULEWEB_CONSTANT_TYPE_FUNCTION_REF].size; i++) {
        if (list->module->constant_pool[MODULEWEB_CONSTANT_TYPE_FUNCTION_REF].pool[i].index == index) {
            return &list->module->constant_pool[MODULEWEB_CONSTANT_TYPE_FUNCTION_REF].pool[i].info.function_ref_info;
        }
    }

    return NULL;
}

static int moduleweb_insn_list_verify_instruction(moduleweb_insn_list* list, moduleweb_instream* stream, bool wide) {
    u8 opcode;
    if (moduleweb_instream_read_u8(stream, &opcode)) {
        return -1;
    }

    switch (opcode) {
        case NOP:
            break;
        case ADD:
        case SUB:
        case MUL:
        case DIV:
        case REM:
            moduleweb_insn_list_do_pop(list, 2);
            moduleweb_insn_list_do_push(list, 1);
            break;
        case LADD:
        case LSUB:
        case LMUL:
        case LDIV:
        case LREM:
            moduleweb_insn_list_do_pop(list, 4); // 4 = 2 longs
            moduleweb_insn_list_do_push(list, 2); // 2 = 1 long
            break;
        case AND:
        case OR:
        case XOR:
            moduleweb_insn_list_do_pop(list, 2);
            moduleweb_insn_list_do_push(list, 1);
            break;
        case LAND:
        case LOR:
        case LXOR:
            moduleweb_insn_list_do_pop(list, 4);
            moduleweb_insn_list_do_push(list, 2);
            break;
        case NOT:
        case NEG:
            moduleweb_insn_list_do_pop(list, 1);
            moduleweb_insn_list_do_push(list, 1);
            break;
        case LNOT:
        case LNEG:
            moduleweb_insn_list_do_pop(list, 2);
            moduleweb_insn_list_do_push(list, 2);
            break;
        case JMP_ICMPEQ:
        case JMP_ICMPNE:
        case JMP_ICMPLT:
        case JMP_ICMPGT:
        case JMP_ICMPLE:
        case JMP_ICMPGE: {
            i16 branch;

            if (moduleweb_instream_read_u16(stream, (u16*) &branch)) {
                return 1;
            }

            moduleweb_insn_list_verify_jump(list, branch);

            moduleweb_insn_list_do_pop(list, 2);

            break;
        }
        case JMPEQ:
        case JMPNE:
        case JMPLT:
        case JMPGT:
        case JMPLE:
        case JMPGE:
            moduleweb_insn_list_do_pop(list, 1);
        case JMP: {
            i16 branch;

            if (moduleweb_instream_read_u16(stream, (u16*) &branch)) {
                return 1;
            }

            moduleweb_insn_list_verify_jump(list, branch);

            break;
        }
        case ICMP:
            moduleweb_insn_list_do_pop(list, 2);
            moduleweb_insn_list_do_push(list, 1);
            break;
        case LCMP:
            moduleweb_insn_list_do_pop(list, 4);
            moduleweb_insn_list_do_push(list, 1);
            break;
        case BPUSH:
            moduleweb_instream_skip(stream, 1);
            moduleweb_insn_list_do_push(list, 1);
            break;
        case SPUSH:
            moduleweb_instream_skip(stream, 2);
            moduleweb_insn_list_do_push(list, 1);
            break;
        case IPUSH:
            moduleweb_instream_skip(stream, 4);
            moduleweb_insn_list_do_push(list, 1);
            break;
        case LPUSH:
            moduleweb_instream_skip(stream, 8);
            moduleweb_insn_list_do_push(list, 2);
            break;
        case CONST_M1:
        case CONST_0:
        case CONST_1:
            moduleweb_insn_list_do_push(list, 1);
        case LCONST_0:
        case LCONST_1:
            moduleweb_insn_list_do_push(list, 2);
            break;
        case CALL: {
            u32 index = 0;

            if (wide) {
                if (moduleweb_instream_read_u32(stream, &index)) {
                    return 1;
                }
            } else {
                if (moduleweb_instream_read_u16(stream, (u16*) &index)) {
                    return 1;
                }
            }

            moduleweb_constant_function_ref_info* func = moduleweb_insn_list_get_function_ref(list, index);
            if (func == NULL) {
                moduleweb_print("error: weirdly generated call instruction\n");
                exit(1);
            }

            moduleweb_constant_name_info* name = moduleweb_insn_list_get_name(list, func->name_info_index);
            if (name == NULL) {
                moduleweb_print("error: weirdly generated call instruction\n");
                exit(1);
            }

            moduleweb_constant_ascii_info* descriptor = moduleweb_insn_list_get_ascii(list, name->descriptor_index);
            if (descriptor == NULL) {
                moduleweb_print("error: weirdly generated call instruction\n");
                exit(1);
            }

            const char* return_descriptor = NULL;
            u32 return_descriptor_length = 0;

            u16 arguments = 0;
            bool parsing_ref = false;

            for (u32 i = 1; i < descriptor->length; i++) {
                char c = descriptor->bytes[i];

                if (c == ')') {
                    if (i + 1 < descriptor->length) {
                        return_descriptor = (char*) descriptor->bytes + i + 1;
                        return_descriptor_length = descriptor->length - (i + 1);
                    }
                    break;
                }

                if (parsing_ref && c == ';') {
                    parsing_ref = false;
                    arguments += 2; // 2 values because 64 bits
                    continue;
                } else if (parsing_ref) {
                    continue;
                }

                if (c == 'R') {
                    parsing_ref = true;
                    continue;
                }

                if (c == 'L' || c == 'D' || c == 'H') {
                    arguments += 2;
                } else {
                    arguments += 1;
                }
            }

            if (return_descriptor == NULL || return_descriptor_length == 0) {
                moduleweb_print("error: bad function descriptor: '");
                fwrite(descriptor->bytes, 1, descriptor->length, stdout);
                moduleweb_print("' (missing return descriptor)\n");

                exit(1);
            }

            moduleweb_print("debug: ");
            fwrite(descriptor->bytes, 1, descriptor->length, stdout);
            printf(" has %u arguments\n", arguments);

            moduleweb_insn_list_do_pop(list, arguments);

            if (return_descriptor_length == 1 && return_descriptor[0] == 'V') {
            } else {
                char c = return_descriptor[0];
                if (c == 'L' || c == 'D' || c == 'H') {
                    moduleweb_insn_list_do_push(list, 2);
                } else {
                    moduleweb_insn_list_do_push(list, 1);
                }
            }

            break;
        }
        case RETURN:
            break;
        case IRETURN:
            moduleweb_insn_list_do_pop(list, 1);
            break;
        case LRETURN:
            moduleweb_insn_list_do_pop(list, 2);
            break;
        case WIDE:
            return moduleweb_insn_list_verify_instruction(list, stream, true);

        default:
            printf("error: unknown instruction discovered in instruction list: %u\n", opcode);
            exit(1);
    }

    return 0;
}

int moduleweb_insn_list_verify(moduleweb_insn_list* list) {
    moduleweb_instream stream;
    moduleweb_instream_open_buffer(&stream, list->buffer, list->writer_stream.memory.pos, false);

    while (stream.memory.pos < stream.memory.size) {
        if (moduleweb_insn_list_verify_instruction(list, &stream, false)) {
            return 1;
        }
    }

    return 0;
}

void moduleweb_insn_list_patch_labels(moduleweb_insn_list* list) {
    for (u64 i = 0; i < list->forward_label_count; i++) {
        moduleweb_forward_label* label = &list->forward_labels[i];

        u64 symbol = moduleweb_insn_list_get_label(list, label->name)->location - label->origin;

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
                printf("warning: unsupported patch size %u for label: %s\n", label->size, label->name);
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

void moduleweb_insn_list_call(moduleweb_insn_list* list, u8 opcode, const char* module,
                              const char* name, const char* descriptor) {
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

    forward.name = label->name;
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

    if (moduleweb_outstream_write_u16(&list->writer_stream, label->location - list->writer_stream.memory.pos)) {
        return;
    }

    if (!label->location_init) {
        moduleweb_insn_list_forward_label(list, label, 2, -2);
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
