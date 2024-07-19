#pragma once

#include "constpool.h"
#include "function.h"
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

	ConstPool constPool;

	Function* functions;
	u32 functionCount;
	u32 functionIndex;

	Function* currentFunction;

	Stack stack;
	u64 stackFrame;
} JesusVM;

extern JesusVM vm;

void StartVM(u8* bytecode, u32 constPoolSize, u32 functionCount);

void ExitVM();

void VMBeginExecution(nullable() Function* entry);