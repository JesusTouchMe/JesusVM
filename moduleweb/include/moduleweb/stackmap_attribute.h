// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_STACKMAP_ATTRIBUTE_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_STACKMAP_ATTRIBUTE_H

#include "moduleweb/stream.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum moduleweb_stackmap_type_id {
    STACKMAP_TYPE_NONE, // for dual slot locals. tells the verifier that this local should NOT be used
    STACKMAP_TYPE_INT,
    STACKMAP_TYPE_LONG,
    STACKMAP_TYPE_FLOAT,
    STACKMAP_TYPE_DOUBLE,
    STACKMAP_TYPE_HANDLE,
    STACKMAP_TYPE_REFERENCE
} moduleweb_stackmap_type_id;

typedef struct moduleweb_stackmap_entry {
    u16 offset_delta;

    u16 stack_state_entry_count;
    u8* stack_state_entries; // safely convertable to moduleweb_stackmap_type_id

    u16 locals_state_entry_count;
    u8* locals_state_entries; // safely convertable to moduleweb_stackmap_type_id
} moduleweb_stackmap_entry;

typedef struct moduleweb_stackmap_attribute {
    u16 entry_count;
    moduleweb_stackmap_entry* entries;
} moduleweb_stackmap_attribute;

int moduleweb_stackmap_attribute_init(moduleweb_stackmap_attribute* stackmap, moduleweb_instream* stream);
void moduleweb_stackmap_attribute_uninit(moduleweb_stackmap_attribute* stackmap);

#ifdef __cplusplus
}
#endif

#endif //JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_STACKMAP_ATTRIBUTE_H
