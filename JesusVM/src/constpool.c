#include "constpool.h"
#include "module.h"

void NewConstPool(ConstPool* constPool, u32 size) {
	constPool->constants = malloc(sizeof(Constant) * size);
	constPool->size = size;
	constPool->index = 0;
}

Constant* AllocConstant(Module* module) {
	if (module->constPool.index >= module->constPool.size) {
		puts("VM error: attempt to allocate more constants than was specified in file");
		return null;
	}

	return &module->constPool.constants[module->constPool.index++];
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