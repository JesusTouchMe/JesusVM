#include "JesusVM/JesusVM.h"
#include "JesusVM/Module.h"

#include "JesusVM/constpool/ConstantAscii.h"

#include <algorithm>

namespace JesusVM {
	Module::Module( Object* linker, moduleweb_module_info* info)
		: mInfo(info)
        , mLinker(linker)
        , mName(info->name)
		, mConstPool(info->constant_pool_size, info->constant_pool) {
        u16 index = 0;

        while (!moduleweb_attribute_array_get(&info->attributes, "RequiredPlugins", info, &index)) {
            moduleweb_attribute_info* requiredPlugins = &info->attributes.array[index];

            index += 1;

            moduleweb_instream stream;
            if (moduleweb_instream_open_buffer(&stream, requiredPlugins->info, requiredPlugins->length, false)) {
                // TODO: linkage error
                std::cout << "error: failed to stream RequiredPlugins attribute data\n";
                std::exit(1);
            }

            u16 count;
            if (moduleweb_instream_read_u16(&stream, &count)) {
                std::cout << "error: failed to read RequiredPlugins attribute data. Potentially malformed\n";
                std::exit(1);
            }

            for (u16 i = 0; i < count; i++) {
                u16 nameIndex = 0;
                if (moduleweb_instream_read_u16(&stream, &nameIndex)) {
                    std::cout << "error: failed to read RequiredPlugins attribute data. Potentially malformed\n";
                    std::exit(1);
                }

                Linker::LoadPlugin(mConstPool.get<ConstantAscii>(nameIndex)->getValue());
            }
        }

        mClasses.reserve(info->class_count);
        mGlobalVars.reserve(info->global_var_count);
        mFunctions.reserve(info->function_count);

        for (u16 i = 0; i < info->class_count; i++) {
            mClasses[mConstPool.get<ConstantAscii>(info->classes[i].name_index)->getValue()] =
                    std::make_unique<Class>(this, &info->classes[i]);
        }

        for (u16 i = 0; i < info->global_var_count; i++) {
            mGlobalVars.emplace_back(this, i);
        }

        for (u16 i = 0; i < info->function_count; i++) {
            mFunctions.emplace_back(this, &info->functions[i]);
        }
	}

    Module::~Module() {
        std::free(mInfo);
    }

    const moduleweb_module_info* Module::getInfo() {
        return mInfo;
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

    GlobalVar* Module::getGlobalVar(std::string_view name, std::string_view descriptor) {
        auto it = std::find_if(mGlobalVars.begin(), mGlobalVars.end(), [name, descriptor](GlobalVar& global) {
            return global.getName() == name && global.getDescriptor() == descriptor;
        });
        if (it != mGlobalVars.end()) return &*it;

        return nullptr;
    }

    GlobalVar* Module::getGlobalVar(ConstantName* name) {
        return getGlobalVar(name->getName(), name->getDescriptor());
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