#include "moduleweb/module_info.h"

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

void moduleweb_function_info_print(moduleweb_function_info* info, const moduleweb_module_info* module, u32 indent) {
    moduleweb_print_indents(indent);

    if (info->modifiers & MODULEWEB_FUNCTION_MODIFIER_PUBLIC) moduleweb_print("public ");
    if (info->modifiers & MODULEWEB_FUNCTION_MODIFIER_PRIVATE) moduleweb_print("private ");
    if (info->modifiers & MODULEWEB_FUNCTION_MODIFIER_PURE) moduleweb_print("pure ");
    if (info->modifiers & MODULEWEB_FUNCTION_MODIFIER_ASYNC) moduleweb_print("async ");
    if (info->modifiers & MODULEWEB_FUNCTION_MODIFIER_NATIVE) moduleweb_print("native ");

    u64 name_length;
    char* name = moduleweb_module_constant_to_string(module, info->name_index, &name_length);

    fwrite(name, 1, name_length, stdout);

    free(name);

    moduleweb_print(" {\n");
    moduleweb_attribute_array_print(&info->attributes, module, indent + 1);
    moduleweb_print_indents(indent);
    moduleweb_print("}");
}
