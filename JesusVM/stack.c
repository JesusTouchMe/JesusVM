#include "pch.h"
#include "stack.h"

void NewStack(Stack* stack, u64 size) {
	stack->data = calloc(size, sizeof(i64));
	stack->typeInfos = calloc((size + 7) / 8, sizeof(u8));
	stack->top = 0;
	stack->size = size;
}

void DeleteStack(Stack* stack) {
	free(stack->data);
	free(stack->typeInfos);
}

void StackPush(Stack* stack, i64 value, bool isObject) {
	if (stack->top >= stack->size) {
		puts("VM error: StackOverflowException");
		exit(1);
	}

	stack->data[stack->top] = value;

	if (isObject) {
		stack->typeInfos[stack->top / 8] |= (1 << (stack->top % 8));
	} else {
		stack->typeInfos[stack->top / 8] &= ~(1 << (stack->top % 8));
	}

	stack->top++;
}

i64 StackPop(Stack* stack, nullable() bool* isObjectBuf) {
	if (stack->top <= 0) {
		puts("VM error: StackUnderflowException");
		exit(1);
	}

	stack->top--;

	if (isObjectBuf != null) *isObjectBuf = StackIsObject(stack, stack->top);
	return stack->data[stack->top];
}

bool StackIsObject(Stack* stack, u64 index) {
	return (stack->typeInfos[index / 8] & (1 << (index % 8))) != 0;
}

void StackSetObject(Stack* stack, u64 index, bool isObject) {
	if (isObject) {
		stack->typeInfos[index / 8] |= (1 << (index % 8));
	} else {
		stack->typeInfos[index / 8] &= ~(1 << (index % 8));
	}
}