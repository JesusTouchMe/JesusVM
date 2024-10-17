#pragma once

#include "function.h"

#include "object/object.h"

typedef struct FunctionReturn {
	bool isObject;
	union {
		Object* object;
		i64 i;
		u64 u;
	};
} FunctionReturn;

void ExecuteCode(u8* codeStart);

FunctionReturn ExecuteFunction(Function* function);
