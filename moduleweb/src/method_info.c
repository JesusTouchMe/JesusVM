// Copyright 2025 JesusTouchMe

#include "moduleweb/method_info.h"
#include "moduleweb/module_info.h"

int moduleweb_method_info_init(moduleweb_method_info* info, PARAM_MUTATED moduleweb_instream* stream) {
    if (moduleweb_instream_read_u16(stream, &info->modifiers)) {
        return 1;
    }

    if (moduleweb_instream_read_u16(stream, &info->name_index)) {
        return 1;
    }

    if (moduleweb_instream_read_u16(stream, &info->function_index)) {
        return 1;
    }

    if (moduleweb_attribute_array_init(&info->attributes, stream)) {
        return 1;
    }

    return 0;
}

void moduleweb_method_info_uninit(moduleweb_method_info* info) {
    moduleweb_attribute_array_uninit(&info->attributes);
}

int moduleweb_method_info_emit_bytes(moduleweb_method_info* info, PARAM_MUTATED moduleweb_outstream* stream) {
    if (moduleweb_outstream_write_u16(stream, info->modifiers)) {
        return 1;
    }

    if (moduleweb_outstream_write_u16(stream, info->name_index)) {
        return 1;
    }

    if (moduleweb_outstream_write_u16(stream, info->function_index)) {
        return 1;
    }

    if (moduleweb_attribute_array_emit_bytes(&info->attributes, stream)) {
        return 1;
    }

    return 0;
}

void moduleweb_method_info_print(moduleweb_method_info* info, const moduleweb_module_info* module, u32 indent) {
    moduleweb_print_indents(indent);

    if (info->modifiers & MODULEWEB_METHOD_MODIFIER_PUBLIC) printf("public ");
    if (info->modifiers & MODULEWEB_METHOD_MODIFIER_PRIVATE) printf("private ");
    if (info->modifiers & MODULEWEB_METHOD_MODIFIER_PROTECTED) printf("protected ");
    if (info->modifiers & MODULEWEB_METHOD_MODIFIER_FINAL) printf("final ");

    u64 name_length;
    char* name = moduleweb_module_constant_to_string(module, info->name_index, &name_length);

    fwrite(name, 1, name_length, stdout);

    free(name);

    moduleweb_print("()");

    moduleweb_print(" {\n");
    moduleweb_attribute_array_print(&info->attributes, module, indent + 1);
    moduleweb_print("\n");
    moduleweb_print_indents(indent);
    moduleweb_print("}");
}
