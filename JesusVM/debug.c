#include "debug.h"
#include "io.h"
#include "util.h"
#include "vm.h"

static String GetFunctionModifiersAsString(u16 modifiers) {
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

void PrintVMDebug() {
	printf("Instruction pointer: %p\n", vm.ip);
	printf("Flags: ");
	PrintBinary(16, vm.flags);

	if (vm.stack.top > 0) {
		printf("Stack frame: %llu\n", vm.stackFrame);
		printf("Stack top: %llu\n", vm.stack.top);
		puts("\nStack (top to bottom):");

		for (i64 i = vm.stack.top - 1; i >= 0; i--) {
			printf("%llu (%s)\n", vm.stack.data[i], StackIsObject(&vm.stack, i) ? "objectref" : "primitive");
		}
	}

	if (vm.functionCount > 0) {
		puts("\nFunctions:");

		for (u64 i = 0; i < vm.functionCount; i++) {
			Function* function = &vm.functions[i];

			Printf(str("%s%s %s("), GetFunctionModifiersAsString(function->modifiers), GetTypeAsString(function->type->returnType), function->name);

			if (function->paramCount > 0) {
				for (u64 j = 0; j < function->paramCount; j++) {
					Printf(str("%s"), GetTypeAsString(function->type->argumentTypes[j]));

					if (j + 1 != function->paramCount) {
						printf(", ");
					}
				}
			}

			printf(");\n");
		}
	}
}