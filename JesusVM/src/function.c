#include "function.h"
#include "vm.h"

static bool ValidateType(FunctionType* type) {
	return type->returnType != null && (type->argumentCount > 0 ? type->argumentTypes != null : true);
}

static bool ValidateAccessModifiers(u16 modifiers) {
	return (modifiers & (MODIFIER_PUBLIC | MODIFIER_PRIVATE)) && !(modifiers & (modifiers - 1));
}

Function* AllocFunction() {
	if (vm.functionIndex >= vm.functionCount) {
		puts("VM error: attempt to allocate more functions than was specified in file");
		return null;
	}

	return &vm.functions[vm.functionIndex++];
}

bool ValidateFunction(Function* function) {
	return ValidateType(function->type) && ValidateAccessModifiers(function->modifiers);
}