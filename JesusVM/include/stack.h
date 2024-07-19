#pragma once

#include "pch.h"

#define OBJECT 1
#define PRIMITIVE 0

typedef struct Stack {
	i64* data; // a pointer to the bottom so it can be freed
	u8* typeInfos; // bit field that tells if an element is a object ref or a primitive value
	u64 top; // index of the next element of the stack
	u64 size;
} Stack;

void NewStack(Stack* stack, u64 size);

void DeleteStack(Stack* stack);

void StackPush(Stack* stack, i64 value, bool isObject);

i64 StackPop(Stack* stack, nullable() bool* isObjectBuf);

bool StackIsObject(Stack* stack, u64 index);

void StackSetObject(Stack* stack, u64 index, bool isObject);