#include "JesusVM/Function.h"
#include "JesusVM/Module.h"

#include "JesusVM/constpool/ConstantAscii.h"
#include "JesusVM/constpool/ConstantName.h"

#include "moduleweb/module_info.h"

namespace JesusVM {
	Function::Function(Module* module, moduleweb_function_info* info)
		: mInfo(info)
        , mModule(module)
		, mModifiers(info->modifiers) {
        auto name = module->getConstPool().get<ConstantName>(info->name_index);

        mName = name->getName();
        mDescriptor = name->getDescriptor();

        if (ParseFunctionType(this)) {
            std::cout << "Bad function descriptor\n";
            std::exit(1);
        }

        mArgumentTypes.shrink_to_fit();

        if (isNative()) {
            mLocalCount = mArgumentTypes.size();
            mStackSize = 0;
            mEntry = nullptr;
            return;
        }

        u16 codeIndex = 0;

        if (moduleweb_attribute_array_get(&info->attributes, "Code", module->getInfo(), &codeIndex)) {
            //TODO: error proper
            std::cout << "Bad function data. No 'Code' attribute defined\n";
            std::exit(1);
        }

        u16 codeIndex2 = codeIndex + 1;

        if (!moduleweb_attribute_array_get(&info->attributes, "Code", module->getInfo(), &codeIndex2)) {
            std::cout << "Bad function data: More than one 'Code' attribute defined\n";
            std::exit(1);
        }

        moduleweb_attribute_info* entry = &info->attributes.array[codeIndex];

        mEntry = entry->info;
        mBytecodeSize = entry->length;
	}

    moduleweb_function_info* Function::getInfo() const {
        return mInfo;
    }

	Module* Function::getModule() const {
		return mModule;
	}

	const TypeInfo& Function::getReturnType() const {
        return mReturnType;
	}

    const std::vector<TypeInfo>& Function::getArgumentTypes() {
        return mArgumentTypes;
    }

	std::string_view Function::getName() const {
		return mName;
	}
	
	std::string_view Function::getDescriptor() const {
		return mDescriptor;
	}
	
	Function::Modifiers Function::getModifiers() const {
		return mModifiers;
	}
	
	u16 Function::getLocalCount() const {
		return mLocalCount;
	}
	
	u16 Function::getStackSize() const {
		return mStackSize;
	}
	
	u8* Function::getEntry() const {
		return mEntry;
	}
	
	u32 Function::getBytecodeSize() const {
		return mBytecodeSize;
	}

    bool Function::isPublic() const {
        return (mModifiers & MODULEWEB_FUNCTION_MODIFIER_PUBLIC) != 0;
    }

    bool Function::isPrivate() const {
        return (mModifiers & MODULEWEB_FUNCTION_MODIFIER_PRIVATE) != 0;
    }

    bool Function::isPure() const {
        return (mModifiers & MODULEWEB_FUNCTION_MODIFIER_PURE) != 0;
    }

    bool Function::isAsync() const {
        return (mModifiers & MODULEWEB_FUNCTION_MODIFIER_ASYNC) != 0;
    }

    bool Function::isNative() const {
        return (mModifiers & MODULEWEB_FUNCTION_MODIFIER_NATIVE) != 0;
    }
}