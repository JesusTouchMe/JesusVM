#pragma once

#include "pch.h"
#include "util/string.h"

#define PRIMITIVE_BYTE_ID NewConstString("b")
#define PRIMITIVE_SHORT_ID NewConstString("s")
#define PRIMITIVE_INT_ID NewConstString("i")
#define PRIMITIVE_LONG_ID NewConstString("l")
#define PRIMITIVE_BOOL_ID NewConstString("B")
#define PRIMITIVE_VOID_ID NewConstString("V")

typedef struct Type {
	bool isPrimitive;
	String id;

	union {
		u64 primitiveSize;
		// pointer to class reference later. gonna be faster than looking up based on the id :D
	};
} Type;

typedef struct FunctionType {
	Type* returnType;
	Type** argumentTypes;
	u64 argumentCount;

	String id;
} FunctionType;

extern Type byteType;
extern Type shortType;
extern Type intType;
extern Type longType;
extern Type boolType;
extern Type voidType;

FunctionType* AllocFunctionType();

void NewFunctionType(FunctionType* functionType, Type* returnType, nullable(if argumentCount <= 0) Type** argumentTypes, u64 argumentCount);

void FreeFunctionTypes();