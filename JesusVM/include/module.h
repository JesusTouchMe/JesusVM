#pragma once

#include "constpool.h"
#include "function.h"

#include "object/class.h"

#include "util/string.h"

#define FUNCTIONS_SECTION_INDEX 0
#define CLASSES_SECTION_INDEX 1
#define CONSTPOOL_SECTION_INDEX 2
#define STRTAB_SECTION_INDEX 3
#define CODE_SECTION_INDEX 4

#define PRIMARY_SECTION_COUNT 5

typedef struct Section {
	String name;
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

	u32 sectionCount;
	Section* sections;
} Module;

Section* AllocSection(Module* module, u32 index);

void FreeSection(Section* section);

Section* GetSectionByName(Module* module, String name);

static inline Section* GetFunctionsSection(Module* module) {
	return &module->sections[FUNCTIONS_SECTION_INDEX];
}

static inline Section* GetClassesSection(Module* module) {
	return &module->sections[CLASSES_SECTION_INDEX];
}

static inline Section* GetConstPoolSection(Module* module) {
	return &module->sections[CONSTPOOL_SECTION_INDEX];
}

static inline Section* GetStrtabSection(Module* module) {
	return &module->sections[STRTAB_SECTION_INDEX];
}

static inline Section* GetCodeSection(Module* module) {
	return &module->sections[CODE_SECTION_INDEX];
}

Module* AllocModule();

void NewModule(Module* module, String name, u32 constPoolSize, u32 classCount, u32 functionCount);

void DeleteModule(Module* module);

Module* OpenModule(String fileName);

String ModuleGetString(Module* module, u32 index);