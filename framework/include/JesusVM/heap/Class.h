#ifndef JESUSVM_CLASS_H
#define JESUSVM_CLASS_H

#include "types.h"

#include "JesusVM/heap/Field.h"

#include "moduleweb/class_info.h"

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

		explicit Class(Module* module);

        void link(moduleweb_class_info* info);
        void linkPrimitive(std::string_view name);
        void linkArray(Class* base, std::string_view name);

        moduleweb_class_info* getInfo() const;
		Module* getModule() const;
		std::string_view getName() const;

	private:
        moduleweb_class_info* mInfo;

        ClassKind mKind;
        Modifiers mModifiers;

		Module* mModule;

		std::string_view mName;

        u64 mMemorySize;

        u32 mFieldCount;
        std::unique_ptr<Field[]> mFields;

        union {
            TypeInfo mRepresentedPrimitive; // for primitive classes, this will hold a pointer to the underlying type
            Class* mArrayBaseClass{}; // for array classes, this will hold a pointer to the base class
        };
	};
}

#endif // JESUSVM_CLASS_H