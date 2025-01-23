#include "moduleweb/class_info.h"
#include "moduleweb/constant_info.h"
#include "moduleweb/module_info.h"

int moduleweb_class_info_init(moduleweb_class_info* info, moduleweb_instream* stream) {
    if (moduleweb_instream_read_u16(stream, &info->name_index)) {
        return 1;
    }

    if (moduleweb_instream_read_u16(stream, &info->modifiers)) {
        return 1;
    }

    if (moduleweb_instream_read_u16(stream, &info->super_class)) {
        return 1;
    }

    if (moduleweb_attribute_array_init(&info->attributes, stream)) {
        return 1;
    }

    if (moduleweb_instream_read_u16(stream, &info->field_count)) {
        moduleweb_attribute_array_uninit(&info->attributes);
        return 1;
    }

    info->fields = malloc(sizeof(moduleweb_field_info) * info->field_count);
    if (info->fields == NULL) {
        moduleweb_attribute_array_uninit(&info->attributes);
        return 1;
    }

    for (u16 i = 0; i < info->field_count; i++) {
        if (moduleweb_field_info_init(&info->fields[i], stream)) {
            moduleweb_attribute_array_uninit(&info->attributes);

            for (u16 j = 0; j < i; j++) {
                moduleweb_field_info_uninit(&info->fields[j]);
            }

            free(info->fields);

            return 1;
        }
    }

    return 0;
}

void moduleweb_class_info_uninit(moduleweb_class_info* info) {
    moduleweb_attribute_array_uninit(&info->attributes);

    for (u16 i = 0; i < info->field_count; i++) {
        moduleweb_field_info_uninit(&info->fields[i]);
    }

    free(info->fields);
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
