#pragma once

#include "function.h"
#include "pch.h"
#include "object/class.h"

typedef struct Module Module;

typedef enum ConstantKind {
	CONST_FUNCTION = 1,
	CONST_CLASS = 2,
	CONST_FIELD = 3,
	CONST_METHOD = 4,
} ConstantKind;

typedef struct Constant {
	ConstantKind kind;
	
	union {
		void* generic;

		Function* function;
		Class* clas;
		FieldRef* field;
		MethodRef* method;
	};
} Constant;

typedef struct ConstPool {
	Constant* constants;
	u32 size;
	u32 index;
} ConstPool;

void NewConstPool(ConstPool* constPool, u32 size);

Constant* AllocConstant(Module* module);

void NewConstFunction(Constant* constant, Function* function);

void NewConstClass(Constant* constant, Class* clas);

void NewConstField(Constant* constant, FieldRef* field);

void NewConstMethod(Constant* constant, MethodRef* method);