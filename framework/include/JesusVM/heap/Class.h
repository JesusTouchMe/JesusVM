#pragma once

#include "types.h"

namespace JesusVM {
	class Module;

	class Class {
	public:
		Class(Module* module, std::string_view name);

		Module* getModule();
		std::string_view getName() const;
	};
}