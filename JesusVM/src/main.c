#include "pch.h"
#include "type.h"
#include "vm.h"

i32 main(i32 argc, char** argv) {
	u8 bytecode[] = {
		// public void main():
		OP_LDI, 0, 0, 0, 34, // x
		OP_LDI, 0, 0, 0, 35, // y
		OP_CALL, 0, 0,

		OP_DEBUG,

		OP_HLT,

		// public int add(int x, int y):
		OP_LOAD, 0,
		OP_LOAD, 1,
		OP_ADD,
		OP_RET,
	};

	StartVM(bytecode, 2, 2);

	Type* testArguments[] = {
		&intType,
		&intType
	};

	Type** mainArguments = null;

	FunctionType* addType = AllocFunctionType();
	NewFunctionType(addType, &intType, testArguments, 2);
	Function* addFunc = AllocFunction();

	addFunc->type = addType;
	addFunc->name = str("add");

	addFunc->modifiers = MODIFIER_PUBLIC;

	addFunc->localCount = 0;
	addFunc->paramCount = 2;

	addFunc->entry = bytecode + 15;

	FunctionType* mainType = AllocFunctionType();
	NewFunctionType(mainType, &voidType, mainArguments, 0);
	Function* mainFunc = AllocFunction();

	mainFunc->type = mainType;
	mainFunc->name = str("main");

	mainFunc->modifiers = MODIFIER_PUBLIC;

	mainFunc->localCount = 0;
	mainFunc->paramCount = 0;
	
	mainFunc->entry = bytecode;

	Constant* testConstant = AllocConstant();
	NewConstFunction(testConstant, addFunc);

	Constant* mainConstant = AllocConstant();
	NewConstFunction(mainConstant, mainFunc);

	VMBeginExecution(mainFunc);

	puts("Bad exit");
	ExitVM();
	return -1;
}