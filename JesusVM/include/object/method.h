#pragma once

#include "util/string.h"

typedef struct Class Class;
typedef struct Function Function;

typedef struct MethodRef {
	Class* thisClass;
	Function* function;

	u32 classMethodIndex;
} MethodRef;

typedef struct Method {
	Function* function;
} Method;

MethodRef* AllocMethodRef(Class* clas, u16 index);

MethodRef* GetMethodRef(Class* clas, String descriptor);