#pragma once

#include "pch.h"
#include "util/string.h"

typedef struct Class Class;
typedef struct Object Object;
typedef struct Type Type;

typedef struct FieldRef {
	Class* thisClass;

	Type* type;
	String name;

	u16 modifiers;

	u32 classFieldIndex; // determined by linker. will always be in that spot in memory relative to an object containing it
} FieldRef;

typedef struct Field {
	Type* type;
	
	union {
		u64 u;
		i64 i;
		Object* ref;
	} value;
} Field;

FieldRef* AllocFieldRef(Class* clas, u16 index);

FieldRef* GetFieldRef(Class* clas, String name);