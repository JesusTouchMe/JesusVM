#ifndef JESUSVM_MODULEWEB_TYPES_H
#define JESUSVM_MODULEWEB_TYPES_H

#include <stdio.h>

#ifdef __cplusplus
#include <cstdint>

#ifndef JESUSVM_TYPES_H
#define JESUSVM_TYPES_H
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;
#endif

#else
#include <stdbool.h>
#include <stdint.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#endif

static inline void moduleweb_print(const char* s) {
    while (*s != 0) {
        putchar(*s);
        s++;
    }
}

static inline void moduleweb_print_indents(u32 indents) { // todo: put in a better file
    for (u32 i = 0; i < indents; i++) {
        moduleweb_print("    ");
    }
}

#endif //JESUSVM_MODULEWEB_TYPES_H
