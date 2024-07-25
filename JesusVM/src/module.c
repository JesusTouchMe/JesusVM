#include "module.h"
#include "vm.h"

static bool CheckModulesCapacity() {
	if (vm.modules == null) return false;

	if (vm.moduleCount >= vm.moduleCapacity) {
		vm.moduleCapacity *= 2;

		Module* newModules = realloc(vm.modules, sizeof(Module) * vm.moduleCapacity);
		if (newModules == null) return false;

		vm.modules = newModules;
	}

	return true;
}

Module* AllocModule() {
	if (!CheckModulesCapacity()) return null;

	return &vm.modules[vm.moduleCount++];
}

void NewModule(Module* module, String name, u32 constPoolSize, u32 functionCount, u8* bytecode) {
	module->name = name;
	
	NewConstPool(&module->constPool, constPoolSize);

	module->functions = malloc(sizeof(Function) * functionCount);
	module->functionCount = functionCount;
	module->functionIndex = 0;

	module->bytecode = bytecode;
}

void DeleteModule(Module* module) {
	free(module->constPool.constants);
	free(module->functions);
}