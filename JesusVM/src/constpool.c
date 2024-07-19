#include "constpool.h"
#include "vm.h"

Constant* AllocConstant() {
	if (vm.constPool.index >= vm.constPool.size) {
		puts("VM error: attempt to allocate more constants than was specified in file");
		return null;
	}

	return &vm.constPool.constants[vm.constPool.index++];
}

void NewConstFunction(Constant* constant, Function* function) {
	constant->kind = CONST_FUNCTION;
	constant->function = function;
}