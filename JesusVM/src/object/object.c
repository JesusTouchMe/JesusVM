#include "object/object.h"
#include "object/heap/heap.h"

void AddReference(Object* object) {
	object->refCount++;
}

void RemoveReference(Object* object) {
	object->refCount--;

	if (object->refCount <= 0) {
		FreeObject(object);
	}
}