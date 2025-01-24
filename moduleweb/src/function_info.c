#include "moduleweb/function_info.h"

int moduleweb_function_info_init(moduleweb_function_info* info, moduleweb_instream* stream) {
    if (moduleweb_instream_read_u16(stream, &info->modifiers)) {
        return 1;
    }

    if (moduleweb_instream_read_u16(stream, &info->name_index)) {
        return 1;
    }

    if (moduleweb_attribute_array_init(&info->attributes, stream)) {
        return 1;
    }

    return 0;
}

void moduleweb_function_info_uninit(moduleweb_function_info* info) {
    moduleweb_attribute_array_uninit(&info->attributes);
}

int moduleweb_function_info_emit_bytes(moduleweb_function_info* info, moduleweb_outstream* stream) {
    if (moduleweb_outstream_write_u16(stream, info->modifiers)) {
        return 1;
    }

    if (moduleweb_outstream_write_u16(stream, info->name_index)) {
        return 1;
    }

    if (moduleweb_attribute_array_emit_bytes(&info->attributes, stream)) {
        return 1;
    }

    return 0;
}
