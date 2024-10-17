#include "debug.h"
#include "vm.h"

#include "object/object.h"
#include "util/util.h"

JesusVM vm = { 0 };

static u8 safetyByteCode[] = {
	OP_DEBUG,
	OP_HLT
};

void StartVM() {
	vm.ip = safetyByteCode;

	vm.modules = malloc(sizeof(Module) * 16);
	vm.moduleCount = 0;
	vm.moduleCapacity = 16;

	NewStack(&vm.stack, 1024);
}

void ExitVM() {
	FreeTypes();

	for (u32 i = 0; i < vm.moduleCount; i++) {
		DeleteModule(&vm.modules[i]);
	}

	free(vm.modules);

	DeleteStack(&vm.stack);

	vm = (JesusVM) {0};
}

void VMBeginExecution(Module* module, nullable() Function* entry) {
	if (entry->paramCount > 0) {
		puts("TODO: param stuff");
		ExitVM();
		exit(2);
	}

	if (entry == null) { // pray we never come to this lmao
		for (u32 i = 0; i < module->functionCount; i++) {
			Function* function = &module->functions[i];

			if (function->type->argumentCount == 0) entry = function;

			String main = str("main");
			if (StringEquals(&function->name, &main)) {
				entry = function;

				if (function->type->argumentCount == 0) {
					break; // perfect match
				}
			}
		}
	}

	vm.currentFunction = entry;
	vm.currentModule = module;
	vm.stack.frame = 0;

	ExecuteCode(entry->entry);
	puts("VMBeginExecution (this should never show)");
}

Module* VMGetModule(String name) {
	// check the opened modules first. reverse iteration because it's likely it'll try to find the latest opened
	for (i64 i = vm.moduleCount - 1; i >= 0; i--) {
		Module* module = &vm.modules[i];

		if (StringEquals(&module->name, &name)) {
			return module;
		}
	}

	return null;
	//TODO: either vm or special moduleloader will know what modules it's allowed to open, but aren't yet opened
}
