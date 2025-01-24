#include <stdio.h>
#include "moduleweb/module_info.h"

static int moduleweb_constant_ascii_info_init(moduleweb_constant_ascii_info* info, moduleweb_instream* stream) {
    if (moduleweb_instream_read_u32(stream, &info->length)) {
        return 1;
    }

    info->bytes = malloc(info->length);
    if (info->bytes == NULL) {
        return 1;
    }

    if (moduleweb_instream_read_bytes(stream, info->bytes, info->length)) {
        free(info->bytes);
        return 1;
    }

    return 0;
}

static int moduleweb_constant_name_info_init(moduleweb_constant_name_info* info, moduleweb_instream* stream) {
    if (moduleweb_instream_read_u16(stream, &info->name_index)) {
        return 1;
    }

    if (moduleweb_instream_read_u16(stream, &info->descriptor_index)) {
        return 1;
    }

    return 0;
}

static int moduleweb_constant_module_ref_info_init(moduleweb_constant_module_ref_info* info, moduleweb_instream* stream) {
    if (moduleweb_instream_read_u16(stream, &info->name_index)) {
        return 1;
    }

    return 0;
}

static int moduleweb_constant_function_ref_info_init(moduleweb_constant_function_ref_info* info, moduleweb_instream* stream) {
    if (moduleweb_instream_read_u16(stream, &info->module_index)) {
        return 1;
    }

    if (moduleweb_instream_read_u16(stream, &info->name_info_index)) {
        return 1;
    }

    return 0;
}

static int moduleweb_constant_class_ref_info_init(moduleweb_constant_class_ref_info* info, moduleweb_instream* stream) {
    if (moduleweb_instream_read_u16(stream, &info->module_index)) {
        return 1;
    }

    if (moduleweb_instream_read_u16(stream, &info->name_index)) {
        return 1;
    }

    return 0;
}

static int moduleweb_constant_field_ref_info_init(moduleweb_constant_field_ref_info* info, moduleweb_instream* stream) {
    if (moduleweb_instream_read_u16(stream, &info->class_index)) {
        return 1;
    }

    if (moduleweb_instream_read_u16(stream, &info->name_info_index)) {
        return 1;
    }

    return 0;
}

static void moduleweb_constant_ascii_info_uninit(moduleweb_constant_ascii_info* info) {
    free(info->bytes);
}

static void moduleweb_constant_name_info_uninit(moduleweb_constant_name_info* info) {

}

static void moduleweb_constant_module_ref_info_uninit(moduleweb_constant_module_ref_info* info) {

}

static void moduleweb_constant_function_ref_info_uninit(moduleweb_constant_function_ref_info* info) {

}

static void moduleweb_constant_class_ref_info_uninit(moduleweb_constant_class_ref_info* info) {

}

static void moduleweb_constant_field_ref_info_uninit(moduleweb_constant_field_ref_info* info) {

}

static int moduleweb_constant_ascii_info_emit_bytes(moduleweb_constant_ascii_info* info, moduleweb_outstream* stream) {
    if (moduleweb_outstream_write_u32(stream, info->length)) {
        return 1;
    }

    if (moduleweb_outstream_write_bytes(stream, info->bytes, info->length)) {
        return 1;
    }

    return 0;
}

static int moduleweb_constant_name_info_emit_bytes(moduleweb_constant_name_info* info, moduleweb_outstream* stream) {
    if (moduleweb_outstream_write_u16(stream, info->name_index)) {
        return 1;
    }

    if (moduleweb_outstream_write_u16(stream, info->descriptor_index)) {
        return 1;
    }

    return 0;
}

static int moduleweb_constant_module_ref_info_emit_bytes(moduleweb_constant_module_ref_info* info, moduleweb_outstream* stream) {
    if (moduleweb_outstream_write_u16(stream, info->name_index)) {
        return 1;
    }

    return 0;
}

static int moduleweb_constant_function_ref_info_emit_bytes(moduleweb_constant_function_ref_info* info, moduleweb_outstream* stream) {
    if (moduleweb_outstream_write_u16(stream, info->module_index)) {
        return 1;
    }

    if (moduleweb_outstream_write_u16(stream, info->name_info_index)) {
        return 1;
    }

    return 0;
}

static int moduleweb_constant_class_ref_info_emit_bytes(moduleweb_constant_class_ref_info* info, moduleweb_outstream* stream) {
    if (moduleweb_outstream_write_u16(stream, info->module_index)) {
        return 1;
    }

    if (moduleweb_outstream_write_u16(stream, info->name_index)) {
        return 1;
    }

    return 0;
}

static int moduleweb_constant_field_ref_info_emit_bytes(moduleweb_constant_field_ref_info* info, moduleweb_outstream* stream) {
    if (moduleweb_outstream_write_u16(stream, info->class_index)) {
        return 1;
    }

    if (moduleweb_outstream_write_u16(stream, info->name_info_index)) {
        return 1;
    }

    return 0;
}

static void moduleweb_constant_ascii_info_print(moduleweb_constant_ascii_info* info, const moduleweb_module_info* module, u32 indent) {
    moduleweb_print("ascii");
    printf("[%u] ", info->length);
    moduleweb_print("\"");
    fwrite(info->bytes, 1, info->length, stdout);
    moduleweb_print("\"");
}

static void moduleweb_constant_name_info_print(moduleweb_constant_name_info* info, const moduleweb_module_info* module, u32 indent) {
    moduleweb_print("name { ");

    u64 name_length;
    char* name = moduleweb_module_constant_to_string(module, info->name_index, &name_length);

    moduleweb_print("\"");
    fwrite(name, 1, name_length, stdout);
    moduleweb_print("\", ");

    free(name);

    name = moduleweb_module_constant_to_string(module, info->descriptor_index, &name_length);

    moduleweb_print("\"");
    fwrite(name, 1, name_length, stdout);
    moduleweb_print("\" }");

    free(name);
}

static void moduleweb_constant_module_ref_info_print(moduleweb_constant_module_ref_info* info, const moduleweb_module_info* module, u32 indent) {
    moduleweb_print("module ");

    u64 name_length;
    char* name = moduleweb_module_constant_to_string(module, info->name_index, &name_length);

    fwrite(name, 1, name_length, stdout);
}

static void moduleweb_constant_function_ref_info_print(moduleweb_constant_function_ref_info* info, const moduleweb_module_info* module, u32 indent) {
    moduleweb_print("function ");

    u64 name_length;
    char* name = moduleweb_module_constant_to_string(module, info->module_index, &name_length);

    fwrite(name, 1, name_length, stdout);
    moduleweb_print("::");

    free(name);

    name = moduleweb_module_constant_to_string(module, info->name_info_index, &name_length);

    fwrite(name, 1, name_length, stdout);

    free(name);
}

static void moduleweb_constant_class_ref_info_print(moduleweb_constant_class_ref_info* info, const moduleweb_module_info* module, u32 indent) {
    moduleweb_print("class ");

    u64 name_length;
    char* name = moduleweb_module_constant_to_string(module, info->module_index, &name_length);

    fwrite(name, 1, name_length, stdout);
    moduleweb_print("::");

    free(name);

    name = moduleweb_module_constant_to_string(module, info->name_index, &name_length);

    fwrite(name, 1, name_length, stdout);

    free(name);
}

static void moduleweb_constant_field_ref_info_print(moduleweb_constant_field_ref_info* info, const moduleweb_module_info* module, u32 indent) {
    moduleweb_print("field ");

    u64 name_length;
    char* name = moduleweb_module_constant_to_string(module, info->class_index, &name_length);

    fwrite(name, 1, name_length, stdout);
    moduleweb_print("::");

    free(name);

    name = moduleweb_module_constant_to_string(module, info->name_info_index, &name_length);

    fwrite(name, 1, name_length, stdout);

    free(name);
}

int moduleweb_constant_info_init(moduleweb_constant_info* info, moduleweb_instream* stream) {
    if (moduleweb_instream_read_u8(stream, &info->type)) {
        return 1;
    }

    switch (info->type) {
        case MODULEWEB_CONSTANT_TYPE_ASCII:
            return moduleweb_constant_ascii_info_init(&info->ascii_info, stream);
        case MODULEWEB_CONSTANT_TYPE_NAME:
            return moduleweb_constant_name_info_init(&info->name_info, stream);
        case MODULEWEB_CONSTANT_TYPE_MODULE_REF:
            return moduleweb_constant_module_ref_info_init(&info->module_ref_info, stream);
        case MODULEWEB_CONSTANT_TYPE_FUNCTION_REF:
            return moduleweb_constant_function_ref_info_init(&info->function_ref_info, stream);
        case MODULEWEB_CONSTANT_TYPE_CLASS_REF:
            return moduleweb_constant_class_ref_info_init(&info->class_ref_info, stream);
        case MODULEWEB_CONSTANT_TYPE_FIELD_REF:
            return moduleweb_constant_field_ref_info_init(&info->field_ref_info, stream);

        default:
            stream->moduleweb_errno = MODULEWEB_ERROR_BAD_CONSTANT_TYPE;
            return 1;
    }
}

void moduleweb_constant_info_uninit(moduleweb_constant_info* info) {
    switch (info->type) {
        case MODULEWEB_CONSTANT_TYPE_ASCII:
            moduleweb_constant_ascii_info_uninit(&info->ascii_info);
            break;
        case MODULEWEB_CONSTANT_TYPE_NAME:
            moduleweb_constant_name_info_uninit(&info->name_info);
            break;
        case MODULEWEB_CONSTANT_TYPE_MODULE_REF:
            moduleweb_constant_module_ref_info_uninit(&info->module_ref_info);
            break;
        case MODULEWEB_CONSTANT_TYPE_FUNCTION_REF:
            moduleweb_constant_function_ref_info_uninit(&info->function_ref_info);
            break;
        case MODULEWEB_CONSTANT_TYPE_CLASS_REF:
            moduleweb_constant_class_ref_info_uninit(&info->class_ref_info);
            break;
        case MODULEWEB_CONSTANT_TYPE_FIELD_REF:
            moduleweb_constant_field_ref_info_uninit(&info->field_ref_info);
            break;

        default:
            fprintf(stderr, "Bad constant type in initialized info struct.");
            break;
    }
}

int moduleweb_constant_info_emit_bytes(moduleweb_constant_info* info, moduleweb_outstream* stream) {
    if (moduleweb_outstream_write_u8(stream, info->type)) {
        return 1;
    }

    switch (info->type) {
        case MODULEWEB_CONSTANT_TYPE_ASCII:
            return moduleweb_constant_ascii_info_emit_bytes(&info->ascii_info, stream);
        case MODULEWEB_CONSTANT_TYPE_NAME:
            return moduleweb_constant_name_info_emit_bytes(&info->name_info, stream);
        case MODULEWEB_CONSTANT_TYPE_MODULE_REF:
            return moduleweb_constant_module_ref_info_emit_bytes(&info->module_ref_info, stream);
        case MODULEWEB_CONSTANT_TYPE_FUNCTION_REF:
            return moduleweb_constant_function_ref_info_emit_bytes(&info->function_ref_info, stream);
        case MODULEWEB_CONSTANT_TYPE_CLASS_REF:
            return moduleweb_constant_class_ref_info_emit_bytes(&info->class_ref_info, stream);
        case MODULEWEB_CONSTANT_TYPE_FIELD_REF:
            return moduleweb_constant_field_ref_info_emit_bytes(&info->field_ref_info, stream);

        default:
            return 1;
    }
}

void moduleweb_constant_info_print(moduleweb_constant_info* info, const moduleweb_module_info* module, u32 indent, u16 index) {
    moduleweb_print_indents(indent);

    printf("[%u] = ", index);

    switch (info->type) {
        case MODULEWEB_CONSTANT_TYPE_ASCII:
            moduleweb_constant_ascii_info_print(&info->ascii_info, module, indent);
            break;
        case MODULEWEB_CONSTANT_TYPE_NAME:
            moduleweb_constant_name_info_print(&info->name_info, module, indent);
            break;
        case MODULEWEB_CONSTANT_TYPE_MODULE_REF:
            moduleweb_constant_module_ref_info_print(&info->module_ref_info, module, indent);
            break;
        case MODULEWEB_CONSTANT_TYPE_FUNCTION_REF:
            moduleweb_constant_function_ref_info_print(&info->function_ref_info, module, indent);
            break;
        case MODULEWEB_CONSTANT_TYPE_CLASS_REF:
            moduleweb_constant_class_ref_info_print(&info->class_ref_info, module, indent);
            break;
        case MODULEWEB_CONSTANT_TYPE_FIELD_REF:
            moduleweb_constant_field_ref_info_print(&info->field_ref_info, module, indent);
            break;

        default:
            printf("bad constant type: %u", info->type);
            break;
    }

}

int moduleweb_constant_info_ascii_compare(moduleweb_constant_ascii_info* s1, const char* s2) {
    u32 s2_length = strlen(s2);

    if (s1->length != s2_length) {
        if (s1->length < s2_length) return -1;
        else return 1;
    }

    return strncmp((const char*) s1->bytes, s2, s2_length);
}