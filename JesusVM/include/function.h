#pragma once

#include "pch.h"
#include "type.h"

#define MODIFIER_PUBLIC 0x0001
#define MODIFIER_PRIVATE 0x0002

typedef struct Module Module;

typedef u16 FunctionModifiers;

typedef struct Function {
	FunctionType* type;
	String name;
	String fullDescriptor; // name(argTypes...)returnType

	Module* module;

	FunctionModifiers modifiers;

	u16 localCount;
	u16 paramCount;

	u8* entry;
} Function;

Function* AllocFunction(Module* module);

bool ValidateFunction(Function* function);

Function* GetFunction(Module* module, String descriptor);