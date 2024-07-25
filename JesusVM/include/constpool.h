#pragma once

#include "function.h"
#include "pch.h"

typedef struct Module Module;

typedef enum ConstantKind {
	CONST_FUNCTION,
} ConstantKind;

typedef struct Constant {
	ConstantKind kind;
	
	union {
		Function* function;
	};
} Constant;

typedef struct ConstPool {
	Constant* constants;
	u32 size;
	u32 index;
} ConstPool;

void NewConstPool(ConstPool* constPool, u32 size);

Constant* AllocConstant(Module* module);

void NewConstFunction(Constant* constant, Function* function);