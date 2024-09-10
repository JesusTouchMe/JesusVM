#include "module.h"
#include "object/class.h"

Class* AllocClass(Module* module, u16 index) {
	return &module->classes[index];
}

Class* GetClass(Module* module, String name) {
	for (i64 i = module->classCount - 1; i >= 0; i--) {
		Class* class = &module->classes[i];

		if (StringEquals(&class->name, &name)) {
			return class;
		}
	}

	return null;
}

static bool ValidateAccessModifiers(u16 modifiers) {
	return (modifiers & (MODIFIER_PUBLIC | MODIFIER_PRIVATE)) && !(modifiers & (modifiers - 1));
}

bool ValidateClass(Class* class) {
	return ValidateAccessModifiers(class->modifiers);
}