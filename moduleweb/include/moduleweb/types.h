#ifndef JESUSVM_MODULEWEB_TYPES_H
#define JESUSVM_MODULEWEB_TYPES_H

#ifdef __cplusplus
#include <cstdio>
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
#include <stdio.h>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#endif

#define MOVE(dest, src) do { (dest) = (src); (src) = 0; } while(0)
#define MOVE_S(dest, src, type) do { (dest) = (src); (src) = (type) {0}; } while(0)

#define VECTOR_ADD(vec_size, vec_capacity, vec_data, item, type) do {           \
    if ((vec_size) >= (vec_capacity)) {                                         \
        (vec_capacity) = ((vec_capacity) == 0) ? 1 : (vec_capacity) * 2;        \
        void* new_data = realloc((vec_data), (vec_capacity) * sizeof(type));    \
        if (new_data == NULL) {                                                 \
            printf("warning: failed to allocate memory for vector");            \
            exit(1);                                                            \
        }                                                                       \
        (vec_data) = new_data;                                                  \
    }                                                                           \
    (vec_data)[(vec_size)++] = (item);                                          \
} while(0)

//
// "Attributes" to make reading function declarations easier :speaking_head:
//

/*
 * Declare a struct member (usually a pointer of sorts) as owned by the struct instance.
 */
#define OWNED_OBJECT

/*
 * Declare a struct member (usually a pointer of sorts) as not owned by the struct instance and should have its lifetime last as long as the object using it.
 */
#define UNOWNED_OBJECT

/*
 * Declare a function parameter object will be copied. Usually into an object via initializer function.
 * Originally passed object won't be referenced after the call.
 */
#define PARAM_COPIED

/*
 * Declare a function parameter object will be moved from the passed pointer to a new, internally stored object.
 * The originally passed object won't be freed, but will likely be zero-set.
 */
#define PARAM_MOVED

/*
 * Declare a function parameter object will have a reference to it after the function call. Usually used in initializers.
 */
#define PARAM_REFERENCED

/*
 * Declare a function parameter pointer to likely be mutated after the function call.
 */
#define PARAM_MUTATED

/*
 * Declare a function parameter to be freed with `free()` after the function call.
 */
#define PARAM_FREED

/*
 * Declare a pointer as nullable without any consequences.
 */
#define NULLABLE

/*
 * If `cond` is true, the `t` attribute will apply, otherwise, the `f` attribute will.
 */
#define IF(cond, t, f)

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
