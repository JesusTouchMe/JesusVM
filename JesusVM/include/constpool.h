#pragma once

#include "function.h"
#include "pch.h"

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

Constant* AllocConstant(); // TODO: make this allocate in module instead of the global vm

void NewConstFunction(Constant* constant, Function* function);