#pragma once

#include "pch.h"
#include "object/class.h"
#include "util/string.h"

#define PRIMITIVE_BYTE_ID NewConstString("b")
#define PRIMITIVE_SHORT_ID NewConstString("s")
#define PRIMITIVE_INT_ID NewConstString("i")
#define PRIMITIVE_LONG_ID NewConstString("l")
#define PRIMITIVE_BOOL_ID NewConstString("B")
#define PRIMITIVE_VOID_ID NewConstString("V")

#define TYPE_BYTE_ID  'b'
#define TYPE_SHORT_ID 's'
#define TYPE_INT_ID   'i'
#define TYPE_LONG_ID  'l'
#define TYPE_BOOL_ID  'B'
#define TYPE_VOID_ID  'V'
#define TYPE_CLASS_ID 'T'

typedef struct Type {
	bool isPrimitive;
	String id;

	union {
		u64 primitiveSize;
		Class* classRef;
	};
} Type;

typedef struct FunctionType {
	Type* returnType;
	Type** argumentTypes;
	u16 argumentCount;

	String id;
} FunctionType;

extern Type byteType;
extern Type shortType;
extern Type intType;
extern Type longType;
extern Type boolType;
extern Type voidType;

void InitTypeSystem();

Type* GetClassType(Class* clas);

FunctionType* AllocFunctionType();

void NewFunctionType(FunctionType* functionType, Type* returnType, nullable(if argumentCount <= 0) Type** argumentTypes, u16 argumentCount);

FunctionType* GetFunctionType(Type* returnType, nullable(if argumentCount <= 0) Type** argumentTypes, u16 argumentCount);

void FreeTypes();