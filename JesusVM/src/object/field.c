#include "object/class.h"
#include "object/field.h"

FieldRef* AllocFieldRef(Class* clas, u16 index) {
	return &clas->fieldRefs[index];
}

FieldRef* GetFieldRef(Class* class, String name) {
	for (i32 i = class->fieldRefCount - 1; i >= 0; i--) {
		FieldRef* field = &class->fieldRefs[i];

		if (StringEquals(&field->name, &name)) {
			return field;
		}
	}

	return null;
}