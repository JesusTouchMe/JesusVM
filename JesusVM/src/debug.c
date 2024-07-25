#include "debug.h"
#include "io.h"
#include "vm.h"

#include "util/util.h"
#include "util/io.h"

static String GetFunctionModifiersAsString(FunctionModifiers modifiers) {
	static char buffer[128];
	memset(buffer, 0, sizeof(buffer));

	if (modifiers & MODIFIER_PUBLIC) {
		strcat_s(buffer, sizeof(buffer), "public ");
	}

	if (modifiers & MODIFIER_PRIVATE) {
		strcat_s(buffer, sizeof(buffer), "private ");
	}

	return NewString(buffer);
}

static String GetTypeAsString(Type* type) {
	if (type->isPrimitive) {
		if (type == &byteType)
			return str("byte");
		else if (type == &shortType)
			return str("short");
		else if (type == &intType)
			return str("int");
		else if (type == &longType)
			return str("long");
		else if (type == &boolType)
			return str("bool");
		else if (type == &voidType)
			return str("void");
		
		puts("VM error: non-existant primitive type");
		return str("primitive-type-error");
	} else {
		return type->id;
	}
}

static void PrintFunction(Function* function) {
	Printf(str("%s%s %s("), GetFunctionModifiersAsString(function->modifiers), GetTypeAsString(function->type->returnType), function->name);

	if (function->paramCount > 0) {
		for (u16 i = 0; i < function->paramCount; i++) {
			Printf(str("%s"), GetTypeAsString(function->type->argumentTypes[i]));

			if (i + 1 < function->paramCount) {
				printf(", ");
			}
		}
	}

	printf(");\n");
}

static void PrintConstant(Constant* constant) {
	switch (constant->kind) {
		case CONST_FUNCTION: {
			printf("(function) ");
			PrintFunction(constant->function);
			break;
		}
	}
}

static void PrintModuleDebug(Module* module) {
	Printf(str("\nModule \"%s\":\n"), module->name);

	if (module->constPool.size > 0) {
		puts("\nConst pool:");

		for (u32 i = 0; i < module->constPool.size; i++) {
			printf("%u: ", i);
			PrintConstant(&module->constPool.constants[i]);
		}
	}

	if (module->functionCount > 0) {
		puts("\nFunctions:");

		for (u32 i = 0; i < module->functionCount; i++) {
			PrintFunction(&module->functions[i]);
		}
	}
}

void PrintVMDebug() {
	printf("Instruction pointer: %p\n", vm.ip);
	printf("Flags: ");
	PrintBinary(16, vm.flags);

	if (vm.stack.top > 0) {
		printf("Stack frame: %llu\n", vm.stackFrame);
		printf("Stack top: %llu\n\n", vm.stack.top);
		puts("Stack (top to bottom):");

		for (i64 i = vm.stack.top - 1; i >= 0; i--) {
			printf("%llu (%s)\n", vm.stack.data[i], StackIsObject(&vm.stack, i) ? "objectref" : "primitive");
		}
	}

	if (vm.moduleCount > 0) {
		printf("\n%u modules loaded.\n", vm.moduleCount);

		for (u32 i = 0; i < vm.moduleCount; i++) {
			PrintModuleDebug(&vm.modules[i]);
		}
	}
}