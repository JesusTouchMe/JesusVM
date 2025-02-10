// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_INSN_BUILDER_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_INSN_BUILDER_H

#include "moduleweb/stream.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_module_builder moduleweb_module_builder;

typedef struct moduleweb_insn_list {
    UNOWNED_OBJECT moduleweb_module_builder* module;

    u64 size;
    OWNED_OBJECT u8* buffer;

    moduleweb_outstream writer_stream;
} moduleweb_insn_list;

void moduleweb_insn_list_add(moduleweb_insn_list* list, u8 opcode);

void moduleweb_insn_list_add_call(moduleweb_insn_list* list, u8 opcode, const char* module, const char* name, const char* descriptor);

void moduleweb_insn_list_add_var(moduleweb_insn_list* list, u8 opcode, u16 local);

#ifdef __cplusplus
}
#endif

#endif //JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_INSN_BUILDER_H
