#include <stdio.h>
#include "moduleweb/module_info.h"

int moduleweb_attribute_info_init(moduleweb_attribute_info* info, moduleweb_instream* stream) {
    if (moduleweb_instream_read_u16(stream, &info->name_index)) {
        return 1;
    }

    if (moduleweb_instream_read_u32(stream, &info->length)) {
        return 1;
    }

    info->info = malloc(info->length);
    if (info->info == NULL) {
        return 1;
    }

    if (moduleweb_instream_read_bytes(stream, info->info, info->length)) {
        free(info->info);
        return 1;
    }

    return 0;
}

void moduleweb_attribute_info_uninit(moduleweb_attribute_info* info) {
    free(info->info);
}

int moduleweb_attribute_info_emit_bytes(moduleweb_attribute_info* info, moduleweb_outstream* stream) {
    if (moduleweb_outstream_write_u16(stream, info->name_index)) {
        return 1;
    }

    if (moduleweb_outstream_write_u32(stream, info->length)) {
        return 1;
    }

    if (moduleweb_outstream_write_bytes(stream, info->info, info->length)) {
        return 1;
    }

    return 0;
}

void moduleweb_attribute_info_print(moduleweb_attribute_info* info, const moduleweb_module_info* module, u32 indent) {
    bool printValue = true;

    for (u32 i = 0; i < info->length; i++) {
        if (info->info[i] < 32 || info->info[i] > 126) {
            printValue = false;
            break;
        }
    }

    char* value;
    u64 value_length;

    if (printValue) {
        value = (char*) info->info;
        value_length = info->length;
    } else {
        static char static_value[32];
        sprintf(static_value, "(unintelligible[%u])", info->length);

        value = static_value;
        value_length = strlen(static_value);
    }

    u64 name_length;
    char* name = moduleweb_module_constant_to_string(module, info->name_index, &name_length);

    moduleweb_print_indents(indent);

    fwrite(name, 1, name_length, stdout);
    moduleweb_print(": ");
    fwrite(value, 1, value_length, stdout);

    free(name);
}

int moduleweb_attribute_array_init(moduleweb_attribute_array* array, moduleweb_instream* stream) {
    if (moduleweb_instream_read_u16(stream, &array->size)) {
        return 1;
    }

    array->array = malloc(sizeof(moduleweb_attribute_info) * array->size);
    if (array->array == NULL) {
        return 1;
    }

    for (u16 i = 0; i < array->size; i++) {
        if (moduleweb_attribute_info_init(&array->array[i], stream)) {
            for (u16 j = 0; j < i; j++) {
                moduleweb_attribute_info_uninit(&array->array[j]);
            }
            return 1;
        }
    }

    return 0;
}

void moduleweb_attribute_array_uninit(moduleweb_attribute_array* array) {
    for (u16 i = 0; i < array->size; i++) {
        moduleweb_attribute_info_uninit(&array->array[i]);
    }
    free(array->array);
}

int moduleweb_attribute_array_emit_bytes(moduleweb_attribute_array* array, moduleweb_outstream* stream) {
    if (moduleweb_outstream_write_u16(stream, array->size)) {
        return 1;
    }

    for (u16 i = 0; i < array->size; i++) {
        if (moduleweb_attribute_info_emit_bytes(&array->array[i], stream)) {
            return 1;
        }
    }

    return 0;
}

void moduleweb_attribute_array_print(moduleweb_attribute_array* array, const moduleweb_module_info* module, u32 indent) {
    moduleweb_print_indents(indent);

    moduleweb_print("Attributes {\n");

    u32 element_indent = indent + 1;
    for (u16 i = 0; i < array->size; i++) {
        moduleweb_attribute_info_print(&array->array[i], module, element_indent);
        moduleweb_print("\n");
    }

    moduleweb_print_indents(indent);
    moduleweb_print("}");
}

int moduleweb_attribute_array_get(const moduleweb_attribute_array* array, const char* name, const moduleweb_module_info* module, u16* index) {
    for (u16 i = *index; i < array->size; i++) {
        moduleweb_constant_ascii_info* attribute_name;
        if (moduleweb_module_constant_get_ascii(module, array->array[i].name_index, &attribute_name)) {
            return 1;
        }

        if (moduleweb_constant_info_ascii_compare(attribute_name, name)) {
            continue;
        }

        *index = i;
        return 0;
    }

    return 1;
}