#include "moduleweb/module_info.h"

int moduleweb_module_info_init(moduleweb_module_info* info, const char* name, moduleweb_instream* stream) {
    if (moduleweb_instream_read_u32(stream, &info->magic)) {
        goto error;
    }

    if (info->magic != MODULEWEB_MAGIC_NUMBER) {
        stream->moduleweb_errno = MODULEWEB_ERROR_BAD_MAGIC;
        goto error;
    }

    if (moduleweb_instream_read_u16(stream, &info->bytecode_version)) {
        goto error;
    }

    info->name = strdup(name);
    if (info->name == NULL) {
        goto error;
    }

    if (moduleweb_attribute_array_init(&info->attributes, stream)) {
        goto error_name;
    }

    if (moduleweb_instream_read_u16(stream, &info->constant_pool_size)) {
        goto error_attributes;
    }

    info->constant_pool = malloc(sizeof(moduleweb_constant_info) * info->constant_pool_size);
    if (info->constant_pool == NULL) {
        goto error_attributes;
    }

    u16 constant_pool_i;
    for (u16 i = 1; i < info->constant_pool_size; i++) {
        if (moduleweb_constant_info_init(&info->constant_pool[i], stream)) {
            constant_pool_i = i;
            goto error_const_pool_init;
        }
    }

    constant_pool_i = info->constant_pool_size;

    if (moduleweb_instream_read_u16(stream, &info->class_count)) {
        goto error_const_pool_init;
    }

    info->classes = malloc(sizeof(moduleweb_class_info) * info->class_count);
    if (info->classes == NULL) {
        goto error_const_pool_init;
    }

    u16 classes_i;
    for (u16 i = 0; i < info->class_count; i++) {
        if (moduleweb_class_info_init(&info->classes[i], stream)) {
            classes_i = i;
            goto error_classes_init;
        }
    }

    classes_i = info->class_count;

    if (moduleweb_instream_read_u16(stream, &info->function_count)) {
        goto error_classes_init;
    }

    info->functions = malloc(sizeof(moduleweb_function_info) * info->function_count);
    if (info->functions == NULL) {
        goto error_classes_init;
    }

    u16 functions_i;
    for (u16 i = 0; i < info->function_count; i++) {
        if (moduleweb_function_info_init(&info->functions[i], stream)) {
            functions_i = i;
            goto error_functions_init;
        }
    }

    functions_i = info->class_count;

    if (!moduleweb_instream_is_eof(stream)) {
        stream->moduleweb_errno = MODULEWEB_ERROR_EXPECTED_EOF;
        goto error_functions_init;
    }

    return 0;

    error_functions_init:
    for (u16 i = 0; i < functions_i; i++) {
        moduleweb_function_info* function = &info->functions[i];
        moduleweb_function_info_uninit(function);
    }

    error_classes_init:
    for (u16 i = 0; i < classes_i; i++) {
        moduleweb_class_info* class = &info->classes[i];
        moduleweb_class_info_uninit(class);
    }

    error_const_pool_init:
    for (u16 i = 1; i < constant_pool_i; i++) {
        moduleweb_constant_info* constant = &info->constant_pool[i];
        moduleweb_constant_info_uninit(constant);
    }

    free(info->constant_pool);

    error_attributes:
    moduleweb_attribute_array_uninit(&info->attributes);

    error_name:
    free(info->name);

    error:
    return 1;
}

void moduleweb_module_info_uninit(moduleweb_module_info* info) {
    free(info->name);

    moduleweb_attribute_array_uninit(&info->attributes);

    for (u16 i = 1; i < info->constant_pool_size; i++) {
        moduleweb_constant_info_uninit(&info->constant_pool[i]);
    }
    free(info->constant_pool);

    for (u16 i = 0; i < info->class_count; i++) {
        moduleweb_class_info_uninit(&info->classes[i]);
    }
    free(info->classes);

    for (u16 i = 0; i < info->function_count; i++) {
        moduleweb_function_info_uninit(&info->functions[i]);
    }
    free(info->functions);
}

int moduleweb_module_info_emit_bytes(moduleweb_module_info* info, moduleweb_outstream* stream) {
    if (moduleweb_outstream_write_u32(stream, info->magic)) {
        return 1;
    }

    if (moduleweb_outstream_write_u16(stream, info->bytecode_version)) {
        return 1;
    }

    if (moduleweb_attribute_array_emit_bytes(&info->attributes, stream)) {
        return 1;
    }

    if (moduleweb_outstream_write_u16(stream, info->constant_pool_size)) {
        return 1;
    }

    for (u16 i = 0; i < info->constant_pool_size; i++) {
        if (moduleweb_constant_info_emit_bytes(&info->constant_pool[i], stream)) {
            return 1;
        }
    }

    if (moduleweb_outstream_write_u16(stream, info->class_count)) {
        return 1;
    }

    for (u16 i = 0; i < info->class_count; i++) {
        if (moduleweb_class_info_emit_bytes(&info->classes[i], stream)) {
            return 1;
        }
    }

    if (moduleweb_outstream_write_u16(stream, info->function_count)) {
        return 1;
    }

    for (u16 i = 0; i < info->function_count; i++) {
        if (moduleweb_function_info_emit_bytes(&info->functions[i], stream)) {
            return 1;
        }
    }

    return 0;
}

void moduleweb_module_info_print(moduleweb_module_info* info, u32 indent) {
    moduleweb_print_indents(indent);


    printf("module %s {\n", info->name);

    indent++;

    moduleweb_print_indents(indent);
    printf("magic: %x\n", info->magic);
    moduleweb_print_indents(indent);
    printf("version: %u\n", info->bytecode_version);

    moduleweb_attribute_array_print(&info->attributes, info, indent);

    moduleweb_print("\n\n");
    moduleweb_print_indents(indent);
    moduleweb_print("const_pool {\n");

    u32 element_indent = indent + 1;
    for (u16 i = 0; i < info->constant_pool_size; i++) {
        moduleweb_constant_info_print(&info->constant_pool[i], info, element_indent, i);
        moduleweb_print("\n");
    }

    moduleweb_print_indents(indent);
    moduleweb_print("}\n\n");

    moduleweb_print_indents(indent);
    moduleweb_print("classes {\n");

    for (u16 i = 0; i < info->class_count; i++) {
        moduleweb_class_info_print(&info->classes[i], info, element_indent);
        moduleweb_print("\n");
    }

    moduleweb_print_indents(indent);
    moduleweb_print("}\n\n");

    moduleweb_print_indents(indent);
    moduleweb_print("functions {\n");

    for (u16 i = 0; i < info->function_count; i++) {
        moduleweb_function_info_print(&info->functions[i], info, element_indent);
        moduleweb_print("\n");
    }

    moduleweb_print_indents(indent);
    moduleweb_print("}\n");

    indent--;

    moduleweb_print_indents(indent);
    moduleweb_print("}\n");
}

static int moduleweb_module_constant_check_bounds(const moduleweb_module_info* module, u16 index) {
    if (index < 1 || index >= module->constant_pool_size) {
        return 1;
    }
    return 0;
}

#define DEFINE_CONSTANT_GETTER(_type, tag, union_member)                                                            \
int moduleweb_module_constant_get_##union_member(const moduleweb_module_info* module, u16 i, _type** result) {      \
    if (moduleweb_module_constant_check_bounds(module, i)) {                                                        \
        return 1;                                                                                                   \
    }                                                                                                               \
                                                                                                                    \
    moduleweb_constant_info* info = &module->constant_pool[i];                                                      \
    if (info->type != tag) {                                                                                        \
        return 1;                                                                                                   \
    }                                                                                                               \
                                                                                                                    \
    *result = &info->union_member##_info;                                                                           \
    return 0;                                                                                                       \
}

DEFINE_CONSTANT_GETTER(moduleweb_constant_ascii_info, MODULEWEB_CONSTANT_TYPE_ASCII, ascii)
DEFINE_CONSTANT_GETTER(moduleweb_constant_name_info, MODULEWEB_CONSTANT_TYPE_NAME, name)
DEFINE_CONSTANT_GETTER(moduleweb_constant_module_ref_info, MODULEWEB_CONSTANT_TYPE_MODULE_REF, module_ref)
DEFINE_CONSTANT_GETTER(moduleweb_constant_function_ref_info, MODULEWEB_CONSTANT_TYPE_FUNCTION_REF, function_ref)
DEFINE_CONSTANT_GETTER(moduleweb_constant_class_ref_info, MODULEWEB_CONSTANT_TYPE_CLASS_REF, class_ref)
DEFINE_CONSTANT_GETTER(moduleweb_constant_field_ref_info, MODULEWEB_CONSTANT_TYPE_FIELD_REF, field_ref)

char* moduleweb_module_constant_to_string(const moduleweb_module_info* module, u16 i, u64* length) {
    if (moduleweb_module_constant_check_bounds(module, i)) {
        return strdup("'constant index out of bounds'");
    }

    moduleweb_constant_info* info = &module->constant_pool[i];

    switch (info->type) {
        case MODULEWEB_CONSTANT_TYPE_ASCII: {
            char* res = malloc(info->ascii_info.length);
            memcpy(res, info->ascii_info.bytes, info->ascii_info.length);

            *length = info->ascii_info.length;

            return res;
        }

        case MODULEWEB_CONSTANT_TYPE_NAME: {
            moduleweb_constant_ascii_info* name_ascii;
            moduleweb_constant_ascii_info* descriptor_ascii;

            if (moduleweb_module_constant_get_ascii(module, info->name_info.name_index, &name_ascii)) {
                return strdup("'invalid constant type given to name_info::name_index'");
            }

            if (moduleweb_module_constant_get_ascii(module, info->name_info.descriptor_index, &descriptor_ascii)) {
                return strdup("'invalid constant type given to name_info::descriptor_index'");
            }

            u64 res_length = name_ascii->length + descriptor_ascii->length;
            char* res = malloc(res_length);

            memcpy(res, name_ascii->bytes, name_ascii->length);
            memcpy(res + name_ascii->length, descriptor_ascii->bytes, descriptor_ascii->length);

            *length = res_length;

            return res;
        }

        case MODULEWEB_CONSTANT_TYPE_MODULE_REF: {
            moduleweb_constant_ascii_info* name_ascii;
            if (moduleweb_module_constant_get_ascii(module, info->module_ref_info.name_index, &name_ascii)) {
                return strdup("'invalid constant type given to module_ref_info::name_index'");
            }

            char* res = malloc(name_ascii->length);
            memcpy(res, name_ascii->bytes, name_ascii->length);

            *length = name_ascii->length;

            return res;
        }

        case MODULEWEB_CONSTANT_TYPE_FUNCTION_REF: {
            u64 module_length;
            char* module_name = moduleweb_module_constant_to_string(module, info->function_ref_info.module_index, &module_length);

            u64 name_length;
            char* name = moduleweb_module_constant_to_string(module, info->function_ref_info.name_info_index, &name_length);

            char* res = malloc(module_length + 2 + name_length);

            memcpy(res, module_name, module_length);
            memcpy(res + module_length, "::", 2);
            memcpy(res + module_length + 2, name, name_length);

            free(module_name);
            free(name);

            *length = module_length + 2 + name_length;

            return res;
        }

        case MODULEWEB_CONSTANT_TYPE_CLASS_REF: {
            u64 module_length;
            char* module_name = moduleweb_module_constant_to_string(module, info->class_ref_info.module_index, &module_length);

            u64 name_length;
            char* name = moduleweb_module_constant_to_string(module, info->class_ref_info.name_index, &name_length);

            char* res = malloc(module_length + 2 + name_length);

            memcpy(res, module_name, module_length);
            memcpy(res + module_length, "::", 2);
            memcpy(res + module_length + 2, name, name_length);

            free(module_name);
            free(name);

            *length = module_length + 2 + name_length;

            return res;
        }

        case MODULEWEB_CONSTANT_TYPE_FIELD_REF: {
            u64 class_length;
            char* class_name = moduleweb_module_constant_to_string(module, info->field_ref_info.class_index, &class_length);

            u64 name_length;
            char* name = moduleweb_module_constant_to_string(module, info->field_ref_info.name_info_index, &name_length);

            char* res = malloc(class_length + 2 + name_length);

            memcpy(res, class_name, class_length);
            memcpy(res + class_length, "::", 2);
            memcpy(res + class_length + 2, name, name_length);

            free(class_name);
            free(name);

            *length = class_length + 2 + name_length;

            return res;
        }

        default:
            return strdup("'invalid constant type'");
    }
}
