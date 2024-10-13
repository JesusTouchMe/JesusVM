#include "object/object.h"

debug_global(u32 totalAllocatedObjects = 0);

Object* AllocObject(Class* class) {
	debug(totalAllocatedObjects++);

	u32 sizeWithoutMethods = sizeof(Object) + class->fieldRefCount * sizeof(Field);
	u32 size = sizeWithoutMethods + class->methodRefCount * sizeof(Method);

	Object* obj = malloc(size);
	if (obj == null) {
		puts("failed allocation");
		return null;
	}

	obj->methods = (u8*) obj + sizeWithoutMethods;

	for (u16 i = 0; i < class->fieldRefCount; i++) {
		obj->fields[i].type = class->fieldRefs[i].type;
		obj->fields[i].value.i = 0;
	}

	for (u16 i = 0; i < class->methodRefCount; i++) {
		obj->methods[i].function = class->methodRefs[i].function;
	}

	obj->type = class;
	obj->refCount = 1; // give a single reference to the caller which it must handle

	return obj;
}

void FreeObject(Object* object) {
	debug(totalAllocatedObjects--);
	free(object);
}

void AddReference(Object* object) {
	object->refCount++;
}

void RemoveReference(Object* object) {
	object->refCount--;

	if (object->refCount <= 0) {
		FreeObject(object);
	}
}