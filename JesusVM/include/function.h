#pragma once

#include "pch.h"
#include "type.h"

#define MODIFIER_PUBLIC 0x0001
#define MODIFIER_PRIVATE 0x0002

typedef u16 FunctionModifiers;

typedef struct Function {
	FunctionType* type;
	String name;

	FunctionModifiers modifiers;

	u16 localCount;
	u16 paramCount;

	u8* entry;
} Function;

Function* AllocFunction(); // TODO: make this allocate in module instead of the global vm

bool ValidateFunction(Function* function);