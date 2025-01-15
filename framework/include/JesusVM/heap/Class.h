#ifndef JESUSVM_CLASS_H
#define JESUSVM_CLASS_H

#include "types.h"

#include "JesusVM/heap/Field.h"

#include <memory>
#include <string_view>

namespace JesusVM {
	class Module;

	class Class {
	public:
		Class(Module* module, std::string_view name);

		Module* getModule();
		std::string_view getName() const;

	private:
		Module* mModule;
		std::string_view mName;

        std::unique_ptr<Field[]> mFields;
	};
}

#endif // JESUSVM_CLASS_H