#include "function.h"
#include "object/class.h"
#include "object/method.h"
#include "util/io.h"

MethodRef* AllocMethodRef(Class* class, u16 index) {
	return &class->methodRefs[index];
}

MethodRef* GetMethodRef(Class* class, String descriptor) {
	for (i32 i = class->methodRefCount - 1; i >= 0; i--) {
		MethodRef* method = &class->methodRefs[i];

		String name = SubStringFrom(&method->function->fullDescriptor, class->name.length + 2);

		if (StringEquals(&name, &descriptor)) {
			return method;
		}
	}

	return null;
}