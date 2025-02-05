#include "JesusVM/JesusVM.h"
#include "JesusVM/Module.h"

#include <algorithm>

namespace JesusVM {
	Module::Module(JesusVM& vm, moduleweb_module_info* info)
		: mModulewebInfo(info)
        , mName(info->name)
		, mConstPool(info->constant_pool_size, info->constant_pool) {
        mClasses.reserve(info->class_count);
        mFunctions.reserve(info->function_count);

        for (u16 i = 0; i < info->class_count; i++) {
            mClasses.emplace_back(&info->classes[i]);
        }

        for (u16 i = 0; i < info->function_count; i++) {
            mFunctions.emplace_back(vm.getTypeSystem(), this, &info->functions[i]);
        }

		for (auto& function : mFunctions) {
			function.mModule = this; // todo: better impl
		}
	}

    const moduleweb_module_info* Module::getInfo() {
        return mModulewebInfo;
    }

	std::string_view Module::getName() const {
		return mName;
	}

	ConstPool& Module::getConstPool() {
		return mConstPool;
	}

	Class* Module::getClass(std::string_view name) {
		auto it = std::find_if(mClasses.begin(), mClasses.end(), [name](Class& clas) { return clas.getName() == name; });
		if (it != mClasses.end()) return &*it;
		
		return nullptr;
	}

	Function* Module::getFunction(std::string_view name, std::string_view descriptor) {
		auto it = std::find_if(mFunctions.begin(), mFunctions.end(),[name, descriptor](Function& function) {
            return function.getName() == name && function.getDescriptor() == descriptor;
        });
		if (it != mFunctions.end()) return &*it;

		return nullptr;
	}

    Function* Module::getFunction(ConstantName* name) {
        return getFunction(name->getName(), name->getDescriptor());
    }
}