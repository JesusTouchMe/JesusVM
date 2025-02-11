#include "JesusVM/JesusVM.h"
#include "JesusVM/Module.h"

#include "JesusVM/constpool/ConstantAscii.h"

#include <algorithm>

namespace JesusVM {
	Module::Module(JesusVM& vm, Object* linker, moduleweb_module_info* info)
		: mInfo(info)
        , mVM(vm)
        , mLinker(linker)
        , mName(info->name)
		, mConstPool(vm, info->constant_pool_size, info->constant_pool) {
        mClasses.reserve(info->class_count);
        mFunctions.reserve(info->function_count);

        for (u16 i = 0; i < info->class_count; i++) {
            mClasses[mConstPool.get<ConstantAscii>(info->classes[i].name_index)->getValue()] =
                    std::make_unique<Class>(this, &info->classes[i]);
        }

        for (u16 i = 0; i < info->function_count; i++) {
            mFunctions.emplace_back(this, &info->functions[i]);
        }

		for (auto& function : mFunctions) {
			function.mModule = this; // todo: better impl
		}
	}

    Module::~Module() {
        std::free(mInfo);
    }

    const moduleweb_module_info* Module::getInfo() {
        return mInfo;
    }

    JesusVM& Module::getVM() const {
        return mVM;
    }

	std::string_view Module::getName() const {
		return mName;
	}

	ConstPool& Module::getConstPool() {
		return mConstPool;
	}

    Class* Module::findClass(std::string_view name) {
        auto it = mClasses.find(name);
        if (it == mClasses.end()) return nullptr;

        return it->second.get();
    }

	Class* Module::getClass(std::string_view name) {
        return Linker::LoadClass(this, name);
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