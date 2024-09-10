#include "type.h"
#include "util/arrayutil.h"
#include "template/vector.h"

DeclareVector(Type);
DeclareVector(FunctionType);

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

static Vector(Type) classTypes;
static Vector(FunctionType) functionTypes;

void NewEmptyString_internal(String* string, u32 length);

void InitTypeSystem() {
	classTypes = NewVector(Type);
	functionTypes = NewVector(FunctionType);
}

Type* GetClassType(Class* class) {
	if (class == null) {
		return null;
	}

	for (u64 i = 0; i < classTypes.size; i++) {
		Type* type = &classTypes.data[i];

		if (type->classRef == class && !type->isPrimitive) {
			return type;
		}
	}

	Type type;
	type.isPrimitive = false;
	type.id = ViewString(&class->name);
	type.classRef = class;
	
	VectorAdd(&classTypes, type);
	return &VectorBack(&classTypes);
}

FunctionType* AllocFunctionType() {
	VectorAdd(&functionTypes, (FunctionType) {0});
	return &VectorBack(&functionTypes);
}

void NewFunctionType(FunctionType* functionType, Type* returnType, nullable(if argumentCount <= 0) Type** argumentTypes, u16 argumentCount) {
	if (functionType == null) {
		return;
	}

	u64 size = sizeof(Type*) * argumentCount;
	Type** arguments = malloc(size);
	memcpy_s(arguments, size, argumentTypes, size);

	functionType->returnType = returnType;
	functionType->argumentTypes = arguments;
	functionType->argumentCount = argumentCount;
}

FunctionType* GetFunctionType(Type* returnType, nullable(if argumentCount <= 0) Type** argumentTypes, u16 argumentCount) {
	for (u64 i = 0; i < functionTypes.size; i++) {
		FunctionType* type = &functionTypes.data[i];

		if (type->returnType == returnType && ArrayEquals(type->argumentTypes, type->argumentCount, argumentTypes, argumentCount)) {
			return type;
		}
	}

	FunctionType type;
	NewFunctionType(&type, returnType, argumentTypes, argumentCount);

	VectorAdd(&functionTypes, type);

	return &VectorBack(&functionTypes);
}

void FreeTypes() {
	for (u64 i = 0; i < functionTypes.size; i++) {
		free(functionTypes.data[i].argumentTypes);
		DeleteString(&functionTypes.data[i].id);
	}

	DeleteVector(&classTypes);
	DeleteVector(&functionTypes);
}