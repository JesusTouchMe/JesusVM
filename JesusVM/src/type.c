#include "type.h"
#include "util/arrayutil.h"
#include "template/vector.h"

typedef Type* TypeP;
typedef FunctionType* FunctionTypeP;

DeclareVector(TypeP);
DeclareVector(FunctionTypeP);

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

static Vector(TypeP) classTypes;
static Vector(FunctionTypeP) functionTypes;

void NewEmptyString_internal(String* string, u32 length);

void InitTypeSystem() {
	classTypes = NewVector(TypeP);
	functionTypes = NewVector(FunctionTypeP);
}

Type* GetClassType(Class* class) {
	if (class == null) {
		return null;
	}

	for (u64 i = 0; i < classTypes.size; i++) {
		Type* type = classTypes.data[i];

		if (type->classRef == class && !type->isPrimitive) {
			return type;
		}
	}

	Type* type = malloc(sizeof(Type));
	type->isPrimitive = false;
	type->id = ViewString(&class->name);
	type->classRef = class;
	
	VectorAdd(&classTypes, type);
	return type;
}

FunctionType* AllocFunctionType() {
	FunctionType* type = malloc(sizeof(FunctionType));
	VectorAdd(&functionTypes, type);

	return type;
}

void NewFunctionType(FunctionType* functionType, Type* returnType, nullable(if argumentCount == 0) Type** argumentTypes, u16 argumentCount) {
	if (functionType == null) {
		return;
	}

	u64 size = sizeof(Type*) * argumentCount;
	Type** arguments = (argumentTypes == null) ? null : malloc(size);
	if (argumentCount > 0) {
		memcpy_s(arguments, size, argumentTypes, size);
	}

	functionType->returnType = returnType;
	functionType->argumentTypes = arguments;
	functionType->argumentCount = argumentCount;
}

static bool FunctionTypeEquals(FunctionType* type, Type* returnType, Type** argumentTypes, u16 argumentCount) {
	if (type->argumentCount != argumentCount) return false;

	for (u16 i = 0; i < argumentCount; i++) {
		if (type->argumentTypes[i] != argumentTypes[i])
			return false;
	}

	return type->returnType == returnType;
}

FunctionType* GetFunctionType(Type* returnType, nullable(if argumentCount <= 0) Type** argumentTypes, u16 argumentCount) {
	for (u64 i = 0; i < functionTypes.size; i++) {
		FunctionType* type = &functionTypes.data[i];

		if (FunctionTypeEquals(type, returnType, argumentTypes, argumentCount)) 
			return type;
	}

	FunctionType* type = AllocFunctionType();
	NewFunctionType(type, returnType, argumentTypes, argumentCount);

	return type;
}

void FreeTypes() {
	for (u64 i = 0; i < classTypes.size; i++) {
		Type* type = classTypes.data[i];

		free(type);
	}

	for (u64 i = 0; i < functionTypes.size; i++) {
		FunctionType* type = functionTypes.data[i];

		free(type->argumentTypes);
		free(type);
	}

	DeleteVector(&classTypes);
	DeleteVector(&functionTypes);
}