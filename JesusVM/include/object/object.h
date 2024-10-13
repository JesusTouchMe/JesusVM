#pragma once

#include "pch.h"
#include "object/class.h"

typedef struct Object {
	Class* type;

	i32 refCount;

	Method* methods;
	Field fields[];
} Object;

Object* AllocObject(Class* clas);

void FreeObject(Object* object);

void AddReference(Object* object);

void RemoveReference(Object* object);