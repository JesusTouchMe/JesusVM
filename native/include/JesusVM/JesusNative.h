#ifndef JESUS_VM_NATIVE_H
#define JESUS_VM_NATIVE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "win32/JesusNative_platform.h"

#define EXTRACT_ARG(typeDesc, argName) argName = args[__COUNTER__].typeDesc

typedef unsigned char	Boolean;
typedef unsigned short	Char;
typedef short			Short;
typedef float			Float;
typedef double			Double;

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
	Boolean Z;
	Byte B;
	Short S;
	Int I;
	Long L;
	JObject R;
};

struct _Field;
typedef struct _Field* FieldRef;

struct _Method;
typedef struct _Method* MethodRef;

struct _Context;
typedef struct _Context* VMContext;

#ifdef __cplusplus
}
#endif

#endif // JESUS_VM_NATIVE_H
