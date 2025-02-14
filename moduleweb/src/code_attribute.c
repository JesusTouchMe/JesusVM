// Copyright 2025 JesusTouchMe

#include "moduleweb/code_attribute.h"

int moduleweb_code_attribute_init(moduleweb_code_attribute* code, moduleweb_instream* stream) {
    if (moduleweb_instream_read_u16(stream, &code->max_locals)) {
        return 1;
    }

    if (moduleweb_instream_read_u16(stream, &code->max_stack_size)) {
        return 1;
    }

    if (moduleweb_instream_read_u32(stream, &code->code_length)) {
        return 1;
    }

    code->code = moduleweb_instream_pointer(stream);

    if (moduleweb_instream_skip(stream, code->code_length)) {
        free(code->code);
        return 1;
    }

    return 0;
}

void moduleweb_code_attribute_uninit(moduleweb_code_attribute* code) {
    free(code->code);
}