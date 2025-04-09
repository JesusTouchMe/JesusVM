// Copyright 2025 JesusTouchMe

#include "moduleweb/global_var_info.h"
#include "moduleweb/module_info.h"

int moduleweb_global_var_info_init(moduleweb_global_var_info* info, PARAM_MUTATED moduleweb_instream* stream) {
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

void moduleweb_global_var_info_uninit(moduleweb_global_var_info* info) {
    moduleweb_attribute_array_uninit(&info->attributes);
}

int moduleweb_global_var_info_emit_bytes(moduleweb_global_var_info* info, PARAM_MUTATED moduleweb_outstream* stream) {
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

void moduleweb_global_var_info_print(moduleweb_global_var_info* info, const moduleweb_module_info* module, u32 indent) {
    moduleweb_print_indents(indent);

    if (info->modifiers & MODULEWEB_GLOBAL_VAR_MODIFIER_PUBLIC) moduleweb_print("public ");
    if (info->modifiers & MODULEWEB_GLOBAL_VAR_MODIFIER_PRIVATE) moduleweb_print("private ");
    if (info->modifiers & MODULEWEB_GLOBAL_VAR_MODIFIER_VOLATILE) moduleweb_print("volatile ");

    u64 name_length;
    char* name = moduleweb_module_constant_to_string(module, info->name_index, &name_length);

    fwrite(name, 1, name_length, stdout);

    free(name);

    moduleweb_print(" {\n");
    moduleweb_attribute_array_print(&info->attributes, module, indent + 1);
    moduleweb_print("\n");
    moduleweb_print_indents(indent);
    moduleweb_print("}");
}