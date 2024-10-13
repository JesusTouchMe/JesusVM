#include "constpool.h"
#include "module.h"
#include "vm.h"

void NewConstPool(ConstPool* constPool, u32 size) {
	constPool->constants = malloc(sizeof(Constant) * size);
	if (constPool->constants == null) {
		puts("malloc fail brah");
		exit(2);
	}

	constPool->size = size;
	constPool->index = 0;

	for (u32 i = 0; i < size; i++) {
		constPool->constants[i].kind = CONST_UNINITIALIZED;
	}
}

Constant* AllocConstant(Module* module) {
	if (module->constPool.index >= module->constPool.size) {
		puts("VM error: attempt to allocate more constants than was specified in file");
		return null;
	}

	return &module->constPool.constants[module->constPool.index++];
}

u32 ConstFromBytes(Constant* constant, Module* module, u8* buffer) {
	ConstantKind kind = (ConstantKind) Immediate8(&buffer);
	switch (kind) {
		case CONST_FUNCTION: {
			u32 moduleIndex = Immediate32(&buffer);
			u32 descriptorIndex = Immediate32(&buffer);

			String moduleName = ModuleGetString(module, moduleIndex);
			String descriptor = ModuleGetString(module, descriptorIndex);

			Module* module = VMGetModule(moduleName);
			Function* function = GetFunction(module, descriptor);

			NewConstFunction(constant, function);
			return 9;
		}

		case CONST_CLASS: {
			u32 moduleIndex = Immediate32(&buffer);
			u32 nameIndex = Immediate32(&buffer);

			String moduleName = ModuleGetString(module, moduleIndex);
			String name = ModuleGetString(module, nameIndex);

			Module* module = VMGetModule(moduleName);
			Class* class = GetClass(module, name);

			NewConstClass(constant, class);
			return 9;
		}

		case CONST_FIELD: {
			u32 moduleIndex = Immediate32(&buffer);
			u32 classIndex = Immediate32(&buffer);
			u32 nameIndex = Immediate32(&buffer);

			String moduleName = ModuleGetString(module, moduleIndex);
			String className = ModuleGetString(module, classIndex);
			String name = ModuleGetString(module, nameIndex);

			Module* module = VMGetModule(moduleName);
			Class* class = GetClass(module, className);
			FieldRef* field = GetFieldRef(class, name);

			NewConstField(constant, field);
			return 13;
		}

		case CONST_METHOD: {
			u32 moduleIndex = Immediate32(&buffer);
			u32 classIndex = Immediate32(&buffer);
			u32 nameIndex = Immediate32(&buffer);

			String moduleName = ModuleGetString(module, moduleIndex);
			String className = ModuleGetString(module, classIndex);
			String name = ModuleGetString(module, nameIndex);

			Module* module = VMGetModule(moduleName);
			Class* class = GetClass(module, className);
			MethodRef* method = GetMethodRef(class, name);

			NewConstMethod(constant, method);
			return 13;
		}

		default: {
			printf("bad constant kind %d\n", kind);
			break;
		}
	}
}

void NewConstFunction(Constant* constant, Function* function) {
	constant->kind = CONST_FUNCTION;
	constant->function = function;
}

void NewConstClass(Constant* constant, Class* class) {
	constant->kind = CONST_CLASS;
	constant->clas = class;
}

void NewConstField(Constant* constant, FieldRef* field) {
	constant->kind = CONST_FIELD;
	constant->field = field;
}

void NewConstMethod(Constant* constant, MethodRef* method) {
	constant->kind = CONST_METHOD;
	constant->method = method;
}

Constant* GetConstant(Module* module, u32 index) {
	if (index >= module->constPool.size) {
		printf("VM error: IndexOutOfBoundsException: constant pool has %u elements, but %u was requested\n", module->constPool.size, index);
		ExitVM();
		exit(1);
	}

	Constant* constant = &module->constPool.constants[index];

	if (constant->kind == CONST_UNINITIALIZED) {
		debug(printf("Initializing constant at %u\n", index));

		u8* buffer = GetConstPoolSection(module)->buffer + index * CONSTANT_ELEMENT_SIZE;
		ConstFromBytes(constant, module, buffer);
	}

	return constant;
}

