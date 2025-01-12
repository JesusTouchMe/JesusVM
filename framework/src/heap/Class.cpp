#include "JesusVM/Module.h"

#include "JesusVM/heap/Class.h"

namespace JesusVM {
	Class::Class(Module* module, std::string_view name) 
		: mModule(module)
		, mName(name) { }

	Module* Class::getModule() {
		return mModule;
	}

	std::string_view Class::getName() const {
		return mName;
	}
}