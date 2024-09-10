#pragma once

#include "pch.h"
#include "object/field.h"
#include "object/method.h"
#include "util/string.h"

typedef struct Module Module;

typedef struct Class {
	String name;

	struct Class* superClass;

	u16 modifiers;

	u16 fieldRefCount;
	u16 methodRefCount;

	FieldRef* fieldRefs;
	MethodRef* methodRefs;
} Class;

Class* AllocClass(Module* module, u16 index);

Class* GetClass(Module* module, String name);

bool ValidateClass(Class* clas);