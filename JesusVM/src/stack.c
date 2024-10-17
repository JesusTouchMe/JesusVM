#include "pch.h"
#include "stack.h"

void NewStack(Stack* stack, u64 size) {
	stack->data = calloc(size, sizeof(i64));
	stack->references = calloc(size, sizeof(bool));
	stack->top = 0;
	stack->size = size;
	stack->frame = 0;
}

void DeleteStack(Stack* stack) {
	for (u64 i = 0; i < stack->top; i++) {
		if (stack->references[i]) {
			Object* obj = (Object*) stack->data[i];
			if (obj != null) RemoveReference(obj);
		}
	}

	free(stack->data);
	free(stack->references);
}

void StackPush(Stack* stack, i64 value) {
	if (stack->top >= stack->size) {
		puts("VM error: StackOverflowException");
		exit(1);
	}

	stack->data[stack->top] = value;
	stack->references[stack->top] = false;
	stack->top++;
}

void StackPushObj(Stack* stack, Object* obj) {
	if (stack->top >= stack->size) {
		puts("VM error: StackOverflowException");
		exit(1);
	}

	stack->data[stack->top] = (i64) obj;
	stack->references[stack->top] = true;
	stack->top++;

	if (obj != null) AddReference(obj);
}

i64 StackPop(Stack* stack) {
	i64 value = StackPop2(stack);

	if (stack->references[stack->top]) {
		Object* obj = (Object*) value;
		if (obj != null) RemoveReference(obj);
	}

	return value;
}

i64 StackPop2(Stack* stack) {
	if (stack->top <= 0) {
		puts("VM error: StackUnderflowException");
		exit(1);
	}

	stack->top--;

	return stack->data[stack->top];
}