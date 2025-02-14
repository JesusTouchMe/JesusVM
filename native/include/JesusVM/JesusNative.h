#ifndef JESUS_VM_NATIVE_H
#define JESUS_VM_NATIVE_H

#ifdef __cplusplus
#include <cstddef>
#endif

#include "win32/JesusNative_platform.h"

#define EXTRACT_ARG(typeDesc, argName) argName = args[__COUNTER__].typeDesc

typedef unsigned char	Bool;
typedef char        	Char;
typedef short			Short;
typedef float			Float;
typedef double			Double;
typedef void*           Handle;

#ifdef __cplusplus

class _Object {};
class _Class {};

typedef _Object* JObject;
typedef _Class* JClass;

#else

struct _Object;
struct _Class;

typedef struct _Object* JObject;
typedef struct _Class* JClass;

#endif

typedef union JValue {
	Bool Z;
	Byte B;
	Short S;
	Int I;
	Long L;
    Char C;
    Float F;
    Double D;
	JObject R;
    Handle H;

#ifdef __cplusplus
    constexpr JValue() : L(0) {}
    constexpr JValue(std::nullptr_t) : H(nullptr) {}

    template<typename T>
    constexpr JValue(T val) {
        if constexpr (std::is_same_v<T, Bool>) {
            Z = val;
        } else if constexpr (std::is_same_v<T, Byte>) {
            B = val;
        } else if constexpr (std::is_same_v<T, Short>) {
            S = val;
        } else if constexpr (std::is_same_v<T, Int>) {
            I = val;
        } else if constexpr (std::is_same_v<T, Long>) {
            L = val;
        } else if constexpr (std::is_same_v<T, Char>) {
            C = val;
        } else if constexpr (std::is_same_v<T, Float>) {
            F = val;
        } else if constexpr (std::is_same_v<T, Double>) {
            D = val;
        } else if constexpr (std::is_same_v<T, JObject>) {
            R = val;
        } else if constexpr (std::is_same_v<T, Handle>) {
            H = val;
        }
    }
#endif
} JValue;

struct _Field;
typedef struct _Field* FieldRef;

struct _Method;
typedef struct _Method* MethodRef;

struct _Context;
typedef struct _Context* VMContext;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif // JESUS_VM_NATIVE_H
