#pragma once

#include "opcodes.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

typedef int8_t	i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t	 u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef u8 bool;

typedef void (*voidfunc)();

#define false 0
#define true 1

#define null NULL
#define nullable(...)

#if defined(__clang__)
#  define COMPILER_CLANG
#elif defined(_MSC_VER)
#  define COMPILER_CL
#elif defined(__GNUC__)
#  define COMPILER_GCC
#endif

#ifdef JESUS_DEBUG
#  define debug(s) do{s;}while(0)
#  define debug_block if(1)
#  define debug_global(s) s
#else
#  define debug(s) do{}while(0)
#  define debug_block if(0)
#  define debug_global(s)
#endif

static inline i8 Immediate8(u8** buffer) {
    i8 value = (i8) (*buffer)[0];
    *buffer += 1;
    return value;
}

static inline i16 Immediate16(u8** buffer) {
    i16 value = (i16) (((*buffer)[0] << 8) | (*buffer)[1]);
    *buffer += 2;
    return value;
}

static inline i32 Immediate32(u8** buffer) {
    i32 value = (i32) (((*buffer)[0] << 24) |
        ((*buffer)[1] << 16) |
        ((*buffer)[2] << 8) |
        (*buffer)[3]);
    *buffer += 4;
    return value;
}

static inline i64 Immediate64(u8** buffer) {
    i64 value = (i64) (((u64) (*buffer)[0] << 56) |
        ((u64) (*buffer)[1] << 48) |
        ((u64) (*buffer)[2] << 40) |
        ((u64) (*buffer)[3] << 32) |
        ((u64) (*buffer)[4] << 24) |
        ((u64) (*buffer)[5] << 16) |
        ((u64) (*buffer)[6] << 8) |
        ((u64) (*buffer)[7]));
    *buffer += 8;
    return value;
}
