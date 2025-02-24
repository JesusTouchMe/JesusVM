// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_INSN_BUILDER_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_INSN_BUILDER_H

#include "moduleweb/stream.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_module_builder moduleweb_module_builder;

typedef struct moduleweb_label {
    OWNED_OBJECT char* name;
    u64 location;
    bool location_init;
} moduleweb_label;

typedef struct moduleweb_forward_label {
    UNOWNED_OBJECT char* name;
    u8 size;
    u64 location;
    u64 origin;
} moduleweb_forward_label;

typedef struct moduleweb_insn_list {
    UNOWNED_OBJECT moduleweb_module_builder* module;
    u8 wide_opcode;

    u64 label_count;
    u64 label_capacity;
    OWNED_OBJECT moduleweb_label** labels;

    u64 forward_label_count;
    u64 forward_label_capacity;
    OWNED_OBJECT moduleweb_forward_label* forward_labels;

    u16 local_count;
    i32 max_stack_depth;

    u64 size;
    OWNED_OBJECT u8* buffer;

    moduleweb_outstream writer_stream;
} moduleweb_insn_list;

int moduleweb_insn_list_init(moduleweb_insn_list* list);

void moduleweb_insn_list_uninit(moduleweb_insn_list* list);


void moduleweb_insn_list_patch_labels(moduleweb_insn_list* list);

moduleweb_label* moduleweb_insn_list_get_label(const moduleweb_insn_list* list, const char* name);

void moduleweb_insn_list_insn(moduleweb_insn_list* list, u8 opcode);

void moduleweb_insn_list_call(moduleweb_insn_list* list, u8 opcode, const char* module,
                              const char* name, const char* descriptor);

moduleweb_label* moduleweb_insn_list_create_label(moduleweb_insn_list* list, NULLABLE const char* name);

void moduleweb_insn_list_add_label(moduleweb_insn_list* list, moduleweb_label* label);

void moduleweb_insn_list_jump(moduleweb_insn_list* list, u8 opcode, moduleweb_label* label);

void moduleweb_insn_list_class(moduleweb_insn_list* list, u8 opcode, const char* module, const char* name);

void moduleweb_insn_list_field(moduleweb_insn_list* list, u8 opcode, const char* owner_module, const char* owner, const char* name, const char* descriptor);

void moduleweb_insn_list_var(moduleweb_insn_list* list, u8 opcode, u16 local);

void moduleweb_insn_list_byte(moduleweb_insn_list* list, u8 opcode, i8 operand);

void moduleweb_insn_list_short(moduleweb_insn_list* list, u8 opcode, i16 operand);

void moduleweb_insn_list_int(moduleweb_insn_list* list, u8 opcode, i32 operand);

void moduleweb_insn_list_long(moduleweb_insn_list* list, u8 opcode, i64 operand);

#ifdef __cplusplus
}
#endif

#endif //JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_BUILDER_INSN_BUILDER_H
