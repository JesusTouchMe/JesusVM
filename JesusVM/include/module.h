#pragma once

#include "constpool.h"
#include "function.h"

#include "util/string.h"

typedef struct Module {
	String name;

	ConstPool constPool;

	Function* functions;
	u32 functionCount;
	u32 functionIndex;

	u8* bytecode;
} Module;

Module* AllocModule();

void NewModule(Module* module, String name, u32 constPoolSize, u32 functionCount, u8* bytecode);

void DeleteModule(Module* module);