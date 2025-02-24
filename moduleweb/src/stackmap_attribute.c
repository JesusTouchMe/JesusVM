// Copyright 2025 JesusTouchMe

#include "moduleweb/stackmap_attribute.h"

int moduleweb_stackmap_attribute_init(moduleweb_stackmap_attribute* stackmap, moduleweb_instream* stream) {
    if (moduleweb_instream_read_u16(stream, &stackmap->entry_count)) {
        return 1;
    }

    stackmap->entries = malloc(sizeof(moduleweb_stackmap_entry) * stackmap->entry_count);
    if (stackmap->entries == NULL) {
        return 1;
    }

    for (u16 i = 0; i < stackmap->entry_count; i++) {
        moduleweb_stackmap_entry* entry = &stackmap->entries[i];

        if (moduleweb_instream_read_u16(stream, &entry->offset_delta)) {
            goto fail;
        }

        if (moduleweb_instream_read_u16(stream, &entry->stack_state_entry_count)) {
            goto fail;
        }

        entry->stack_state_entries = moduleweb_instream_pointer(stream);
        if (moduleweb_instream_skip(stream, entry->stack_state_entry_count)) {
            goto fail;
        }

        entry->locals_state_entries = moduleweb_instream_pointer(stream);
        if (moduleweb_instream_skip(stream, entry->locals_state_entry_count)) {
            goto fail;
        }
    }

    return 0;

    fail:
    free(stackmap->entries);
    return 1;
}

void moduleweb_stackmap_attribute_uninit(moduleweb_stackmap_attribute* stackmap) {
    free(stackmap->entries);
}