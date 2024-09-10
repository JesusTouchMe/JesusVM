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