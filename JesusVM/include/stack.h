#pragma once

#include "pch.h"
#include "object/object.h"

#define OBJECT 1
#define PRIMITIVE 0

typedef struct Stack {
	i64* data; // a pointer to the bottom so it can be freed
	bool* references; // stores a bool for each stack element. a value of true means the stack value at the same index contains an object reference
	u64 top; // index of the next element of the stack
	u64 size;
	u64 frame;
} Stack;

void NewStack(Stack* stack, u64 size);

void DeleteStack(Stack* stack);

void StackPush(Stack* stack, i64 value);

void StackPushObj(Stack* stack, Object* obj); // this function will automatically add a reference to the given object

i64 StackPop(Stack* stack); // will automatically remove a reference if it pops an object

i64 StackPop2(Stack* stack); // StackPop, but doesn't remove reference from an object