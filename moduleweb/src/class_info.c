#include "moduleweb/class_info.h"
#include "moduleweb/constant_info.h"
#include "moduleweb/module_info.h"

int moduleweb_class_info_init(moduleweb_class_info* info, moduleweb_instream* stream) {
    if (moduleweb_instream_read_u16(stream, &info->name_index)) {
        goto error;
    }

    if (moduleweb_instream_read_u16(stream, &info->modifiers)) {
        goto error;
    }

    if (moduleweb_instream_read_u16(stream, &info->super_class)) {
        goto error;
    }

    if (moduleweb_attribute_array_init(&info->attributes, stream)) {
        goto error;
    }

    if (moduleweb_instream_read_u16(stream, &info->field_count)) {
        moduleweb_attribute_array_uninit(&info->attributes);
        return 1;
    }

    info->fields = malloc(sizeof(moduleweb_field_info) * info->field_count);
    if (info->fields == NULL) {
        goto error_attributes;
    }

    for (u16 i = 0; i < info->field_count; i++) {
        if (moduleweb_field_info_init(&info->fields[i], stream)) {
            info->field_count = i;
            goto error_fields;
        }
    }

    if (moduleweb_instream_read_u16(stream, &info->method_count)) {
        goto error_fields;
    }

    info->methods = malloc(sizeof(moduleweb_method_info) * info->method_count);
    if (info->methods == NULL) {
        goto error_fields;
    }

    for (u16 i = 0; i < info->method_count; i++) {
        if (moduleweb_method_info_init(&info->methods[i], stream)) {
            info->method_count = i;
            goto error_methods;
        }
    }

    return 0;

    error_methods:
    for (u16 i = 0; i < info->method_count; i++) {
        moduleweb_method_info_uninit(&info->methods[i]);
    }
    free(info->methods);

    error_fields:
    for (u16 i = 0; i < info->field_count; i++) {
        moduleweb_field_info_uninit(&info->fields[i]);
    }
    free(info->fields);

    error_attributes:
    moduleweb_attribute_array_uninit(&info->attributes);

    error:
    return 1;
}

void moduleweb_class_info_uninit(moduleweb_class_info* info) {
    moduleweb_attribute_array_uninit(&info->attributes);

    for (u16 i = 0; i < info->field_count; i++) {
        moduleweb_field_info_uninit(&info->fields[i]);
    }

    free(info->fields);

    for (u16 i = 0; i < info->method_count; i++) {
        moduleweb_method_info_uninit(&info->methods[i]);
    }

    free(info->methods);
}

int moduleweb_class_info_emit_bytes(moduleweb_class_info* info, moduleweb_outstream* stream) {
    if (moduleweb_outstream_write_u16(stream, info->name_index)) {
        return 1;
    }

    if (moduleweb_outstream_write_u16(stream, info->modifiers)) {
        return 1;
    }

    if (moduleweb_outstream_write_u16(stream, info->super_class)) {
        return 1;
    }

    if (moduleweb_attribute_array_emit_bytes(&info->attributes, stream)) {
        return 1;
    }

    if (moduleweb_outstream_write_u16(stream, info->field_count)) {
        return 1;
    }

    for (u16 i = 0; i < info->field_count; i++) {
        if (moduleweb_field_info_emit_bytes(&info->fields[i], stream)) {
            return 1;
        }
    }

    if (moduleweb_outstream_write_u16(stream, info->method_count)) {
        return 1;
    }

    for (u16 i = 0; i < info->method_count; i++) {
        if (moduleweb_method_info_emit_bytes(&info->methods[i], stream)) {
            return 1;
        }
    }

    return 0;
}

void moduleweb_class_info_print(moduleweb_class_info* info, const moduleweb_module_info* module, u32 indent) {
    u64 name_length;
    char* name = moduleweb_module_constant_to_string(module, info->name_index, &name_length);

    moduleweb_print_indents(indent);

    if (info->modifiers & MODULEWEB_CLASS_MODIFIER_PUBLIC) moduleweb_print("public ");
    if (info->modifiers & MODULEWEB_CLASS_MODIFIER_PRIVATE) moduleweb_print("private ");
    if (info->modifiers & MODULEWEB_CLASS_MODIFIER_FINAL) moduleweb_print("final ");

    moduleweb_print("class ");
    fwrite(name, 1, name_length, stdout);

    free(name);

    if (info->super_class != 0) {
        name = moduleweb_module_constant_to_string(module, info->super_class, &name_length);

        moduleweb_print(" extends ");
        fwrite(name, 1, name_length, stdout);

        free(name);
    }

    moduleweb_print(" {\n");

    u32 element_indent = indent + 1;
    moduleweb_attribute_array_print(&info->attributes, module, element_indent);
    moduleweb_print("\n");

    for (u16 i = 0; i < info->field_count; i++) {
        moduleweb_field_info_print(&info->fields[i], module, element_indent);
        moduleweb_print("\n");
    }

    moduleweb_print("\n");

    for (u16 i = 0; i < info->method_count; i++) {
        moduleweb_method_info_print(&info->methods[i], module, element_indent);
        moduleweb_print("\n");
    }

    moduleweb_print_indents(indent);
    moduleweb_print("}");
}

int moduleweb_class_get_field(const moduleweb_class_info* info, const moduleweb_module_info* module, const char* name, const char* descriptor, moduleweb_field_info** result) {
    for (u16 i = 0; i < info->field_count; i++) {
        moduleweb_constant_name_info* field_full_name;
        if (moduleweb_module_constant_get_name(module, info->fields[i].name_index, &field_full_name)) {
            return 1;
        }

        moduleweb_constant_ascii_info* field_name;
        if (moduleweb_module_constant_get_ascii(module, field_full_name->name_index, &field_name)) {
            return 1;
        }

        if (moduleweb_constant_info_ascii_compare(field_name, name)) {
            continue;
        }

        moduleweb_constant_ascii_info* field_descriptor;
        if (moduleweb_module_constant_get_ascii(module, field_full_name->descriptor_index, &field_descriptor)) {
            return 1;
        }

        if (moduleweb_constant_info_ascii_compare(field_descriptor, descriptor)) {
            continue;
        }

        *result = &info->fields[i];

        return 0;
    }

    return 1;
}
