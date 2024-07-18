#include "type.h"

Type byteType = {
	.isPrimitive = true,
	.id = PRIMITIVE_BYTE_ID,
	.primitiveSize = 1
};

Type shortType = {
	.isPrimitive = true,
	.id = PRIMITIVE_SHORT_ID,
	.primitiveSize = 2
};

Type intType = {
	.isPrimitive = true,
	.id = PRIMITIVE_INT_ID,
	.primitiveSize = 4
};

Type longType = {
	.isPrimitive = true,
	.id = PRIMITIVE_LONG_ID,
	.primitiveSize = 8
};

Type boolType = {
	.isPrimitive = true,
	.id = PRIMITIVE_BOOL_ID,
	.primitiveSize = 1
};

Type voidType = {
	.isPrimitive = true,
	.id = PRIMITIVE_VOID_ID,
	.primitiveSize = 0
};

typedef struct FunctionTypeNode {
	struct FunctionTypeNode* next;
	FunctionType functionType;
} FunctionTypeNode;

static FunctionTypeNode* head = null;

static void GetFunctionTypeID(FunctionType* functionType, String* id) {
	u64 length = functionType->returnType->id.length + 2; // +2 for the ()

	for (u64 i = 0; i < functionType->argumentCount; i++) {
		length += functionType->argumentTypes[i]->id.length;
	}

	id->cString = malloc(length + 1);

	if (id->cString == null) {
		puts("VM error: NullPointerException: allocating space for FunctionType id failed");
		return;
	}

	id->length = length;
	id->cString[0] = '(';

	u64 index = 1;
	for (u64 i = 0; i < functionType->argumentCount; i++) {
		Type* argument = functionType->argumentTypes[i];
		memcpy_s(id->cString + index, length, argument->id.cString, argument->id.length);
		index += argument->id.length;
	}

	id->cString[index++] = ')';

	memcpy_s(id->cString + index, length, functionType->returnType->id.cString, functionType->returnType->id.length);

	id->cString[length] = 0;
}

FunctionType* AllocFunctionType() {
	FunctionTypeNode* node = malloc(sizeof(FunctionTypeNode));
	if (node == null) {
		return null;
	}

	node->next = head;
	head = node;

	return &node->functionType;
}

void NewFunctionType(FunctionType* functionType, Type* returnType, nullable(if argumentCount <= 0) Type** argumentTypes, u64 argumentCount) {
	if (functionType == null) {
		return;
	}

	functionType->returnType = returnType;
	functionType->argumentTypes = argumentTypes;
	functionType->argumentCount = argumentCount;
	GetFunctionTypeID(functionType, &functionType->id);
}

void FreeFunctionTypes() {
	while (head != null) {
		FunctionTypeNode* tmp = head;
		head = head->next;
		free(tmp);
	}
}