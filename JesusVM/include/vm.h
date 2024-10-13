#pragma once

#include "module.h"
#include "pch.h"
#include "stack.h"
#include "type.h"

#define FLAG_EQ 0x0001
#define FLAG_LF 0x0002
#define FLAG_GF 0x0004

#define GetFlag(flagMask) (vm.flags & (flagMask))
#define SetFlag(flagMask) (vm.flags |= (flagMask))

typedef struct JesusVM {
	u8* ip;
	u16 flags;

	Function* currentFunction;
	Module* currentModule;

	Module* modules;
	u32 moduleCount;
	u32 moduleCapacity;

	Stack stack;
} JesusVM;

extern JesusVM vm;

void StartVM();

void ExitVM();

void VMBeginExecution(Module* module, nullable() Function* entry);

Module* VMGetModule(String name);

void ExecuteCode(bool runningCodeBlock, u8* codeStart, u16 localCount);
