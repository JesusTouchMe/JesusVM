#include "JesusVM/Module.h"

#include "JesusVM/constpool/ConstantAscii.h"

#include "JesusVM/heap/Class.h"

namespace JesusVM {
    Class::Class(Module* module)
        : mModule(module) { }

    moduleweb_class_info* Class::getInfo() const {
        return mInfo;
    }

	Module* Class::getModule() const {
		return mModule;
	}

	std::string_view Class::getName() const {
		return mName;
	}
}