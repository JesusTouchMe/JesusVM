#ifndef JESUSVM_CLASS_H
#define JESUSVM_CLASS_H

#include "types.h"

#include "JesusVM/heap/Field.h"

#include <memory>
#include <string_view>

namespace JesusVM {
	class Module;

    enum class ClassKind {
        REGULAR,
        ARRAY, // artificial
        PRIMITIVE // artificial
    };

	class Class {
	public:
        using Modifiers = u16;

		Class(Module* module, std::string_view name);

		Module* getModule();
		std::string_view getName() const;

	private:
        ClassKind mKind;
        Modifiers mModifiers;

		Module* mModule;

		std::string_view mName;

        u64 mMemorySize;

        u32 mFieldCount;
        std::unique_ptr<Field[]> mFields;

        union {
            Type* mRepresentedPrimitive; // for primitive classes, this will hold a pointer to the underlying type
            Class* mArrayBaseClass; // for array classes, this will hold a pointer to the base class
        };
	};
}

#endif // JESUSVM_CLASS_H