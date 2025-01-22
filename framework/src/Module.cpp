#include "JesusVM/Module.h"

#include <algorithm>

namespace JesusVM {


	Module::Module(std::string_view name, u32 constCount, std::vector<Class> classes, std::vector<Function> functions)
		: mName(name)
		, mConstPool(constCount)
		, mClasses(std::move(classes))
		, mFunctions(std::move(functions)) {
		for (auto& function : mFunctions) {
			function.mModule = this; // todo: better impl
		}
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

	Function* Module::getFunction(std::string_view descriptor) {
		auto it = std::find_if(mFunctions.begin(), mFunctions.end(), [descriptor](Function& function) { return function.getDescriptor() == descriptor; });
		if (it != mFunctions.end()) return &*it;

		return nullptr;
	}
}