#pragma once

typedef enum {
	/* Performs no operation */
	OP_NOP,

	/* Pops the top value from the stack and discards it */
	OP_POP,

	/* Duplicates the top of the stack */
	OP_DUP,

	/* Same as LOAD, but uses the stack instead of the accumulator */
	OP_LOAD,

	/* Same as STORE, but uses the stack instead of the accumulator */
	OP_STORE,

	/* 
	Pops 2 values from the stack, adds them and pushes the result back on the stack
	rhs = pop()
	lhs = pop()
	push(lhs + rhs)
	*/
	OP_ADD,

	/*
	Pops 2 values from the stack, subtracts them and pushes the result back on the stack
	rhs = pop()
	lhs = pop()
	push(lhs - rhs)
	*/
	OP_SUB,

	/*
	Pops 2 values from the stack, multiplies them and pushes the result back on the stack
	rhs = pop()
	lhs = pop()
	push(lhs * rhs)
	*/
	OP_MUL,

	/*
	Pops 2 values from the stack, divides them and pushes the result back on the stack
	rhs = pop()
	lhs = pop()
	push(lhs / rhs)
	*/
	OP_DIV,

	/*
	Pops 2 values from the stack, ANDs them and pushes the result back on the stack
	rhs = pop()
	lhs = pop()
	push(lhs & rhs)
	*/
	OP_AND,

	/*
	Pops 2 values from the stack, ORs them and pushes the result back on the stack
	rhs = pop()
	lhs = pop()
	push(lhs | rhs)
	*/
	OP_OR,

	/*
	Pops 2 values from the stack, XORs them and pushes the result back on the stack
	rhs = pop()
	lhs = pop()
	push(lhs ^ rhs)
	*/
	OP_XOR,

	/* Increments the top value on the stack by 1 */
	OP_INC,

	/* Decrements the top value on the stack by 1 */
	OP_DEC,

	/* Performs bitwise NOT on the top value on the stack */
	OP_NOT,

	/* Negates the integer value in the top value on the stack*/
	OP_NEG,

	/* 
	Pops 2 values from the stack, compares them and sets the VM flags accordingly
	rhs = pop()
	lhs = pop()
	flags = cmp(lhs, rhs)
	*/
	OP_CMP,

	/* Increments the instruction pointer by the signed value on the top of the stack */
	OP_JMP,

	/* Increments the instruction pointer by the signed 1 byte immediate following this instruction */
	OP_JMPREL,

	/* Increments the instruction pointer by the signed value on the top of the stack if the EQ flag is set */
	OP_JEQ,

	/* Increments the instruction pointer by the signed 1 byte immediate following this instruction if the EQ flag is set */
	OP_JEQREL,

	/* Increments the instruction pointer by the signed value on the top of the stack if the EQ flag isn't set */
	OP_JNE,

	/* Increments the instruction pointer by the signed 1 byte immediate following this instruction if the EQ flag isn't set */
	OP_JNEREL,

	/* Increments the instruction pointer by the signed value on the top of the stack if the LF flag is set */
	OP_JLT,

	/* Increments the instruction pointer by the signed 1 byte immediate following this instruction if the LF flag is set */
	OP_JLTREL,

	/* Increments the instruction pointer by the signed value on the top of the stack if the GF flag is set */
	OP_JGT,

	/* Increments the instruction pointer by the signed 1 byte immediate following this instruction if the GF flag is set */
	OP_JGTREL,

	/* Increments the instruction pointer by the signed value on the top of the stack if the LF or EQ flag is set */
	OP_JLE,

	/* Increments the instruction pointer by the signed 1 byte immediate following this instruction if the LF or EQ flag is set */
	OP_JLEREL,

	/* Increments the instruction pointer by the signed value on the top of the stack if the GF flag is set */
	OP_JGE,

	/* Increments the instruction pointer by the signed 1 byte immediate following this instruction if the GF or EQ flag is set */
	OP_JGEREL,

	/* Saves the current call context, creates a new stack frame and jumps to the entry of the function located in the constant pool index at the 2 bytes immediate following this instruction */
	OP_CALL,

	/* 
	Restores the current stack frame and saved call context to the values from before a function call. 
	If the current functions return type is non-void, the top of the stack at the point of this instruction being executed will be the return value and is pushed to the stack top of the previous frame 
	*/
	OP_RET,

	/* Gets a value from the constant pool index at the 2 bytes immediate following this instruction and stores it on the top of the stack */
	OP_CONSTLOAD,

	/* Will push the next 4 bytes as an immediate to the stack */
	OP_LDI,

	/*
	Will widen the argument size for any of the following instructions:

	LOAD(_STACK) - from 1 byte immediate to 2 byte immediate
	STORE(_STACK) - from 1 byte immediate to 2 byte immediate
	JxxREL - from 1 byte immediate to 4 byte immediate (xx meaning all the conditional and unconditional jump rel instructions)
	CALL - from 2 byte immediate to 4 byte immediate
	CONSTLOAD(_STACK) - from 2 byte immediate to 4 byte immediate
	LDI(_STACK) - from 4 byte immediate to 8 byte immediate
	*/
	OP_WIDE,

	/* Prints debug stuff to stdout. Will throw an exception if the VM is not in debug mode */
	OP_DEBUG,

	/* Halts program execution and exits the program with the exit code on the top of the stack */
	OP_HLT,
} Opcode;