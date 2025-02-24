// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_STACKMAP_ATTRIBUTE_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_STACKMAP_ATTRIBUTE_H

#include "moduleweb/stream.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum moduleweb_stackmap_local_type {
    STACKMAP_INT,
    STACKMAP_LONG,
    STACKMAP_FLOAT,
    STACKMAP_DOUBLE,
    STACKMAP_HANDLE,
    STACKMAP_REFERENCE
} moduleweb_stackmap_local_type;

#ifdef __cplusplus
}
#endif

#endif //JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_STACKMAP_ATTRIBUTE_H
