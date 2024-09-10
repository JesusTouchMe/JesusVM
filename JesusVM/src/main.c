#include "pch.h"
#include "type.h"
#include "vm.h"
#include "os/init.h"

i32 main(i32 argc, char** argv) {
	OSInit();

	InitTypeSystem();
	StartVM();

	Module* module = OpenModule(str("C:/Users/JesusTouchMe/IdeaProjects/JesusASM/run/test.jmod"));

	Constant* entryConstant = &module->constPool.constants[module->functionIndex];

	if (entryConstant->kind != CONST_FUNCTION) {
		puts("Module entry may only point to a constant pool item with the kind 'FUNCTION'");
		ExitVM();
		return -1;
	}

	VMBeginExecution(module, entryConstant->function);

	puts("Bad exit");
	ExitVM();
	return -1;
}