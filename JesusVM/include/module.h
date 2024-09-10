#pragma once

#include "constpool.h"
#include "function.h"

#include "object/class.h"

#include "util/string.h"

typedef struct Section {
	u32 size;
	u8* buffer;
} Section;

typedef struct Module {
	String name;

	ConstPool constPool;

	Class* classes;
	u32 classCount;

	Function* functions;
	u32 functionCount;
	u32 functionIndex; // doubles as entry index because we effecient like that

	Section stringTable;
	Section bytecode;
} Module;

void FreeSection(Section* section);

Module* AllocModule();

void NewModule(Module* module, String name, u32 constPoolSize, u32 classCount, u32 functionCount, Section stringTab, Section bytecode);

void DeleteModule(Module* module);

Module* OpenModule(String fileName);

String ModuleGetString(Module* module, u32 index);