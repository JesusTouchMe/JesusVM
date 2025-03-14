#include "JesusVM/Function.h"
#include "JesusVM/JesusVM.h"

#include "JesusVM/constpool/ConstantAscii.h"
#include "JesusVM/constpool/ConstantName.h"

#include "moduleweb/module_info.h"

namespace JesusVM {
	Function::Function(Module* module, moduleweb_function_info* info)
		: mInfo(info)
        , mModule(module)
        , mNeededLocalsForArgs(0)
		, mModifiers(info->modifiers) {
        auto name = module->getConstPool().get<ConstantName>(info->name_index);

        mName = name->getName();
        mDescriptor = name->getDescriptor();

        if (ParseFunctionType(this)) {
            std::cout << "Bad function descriptor\n";
            std::exit(1);
        }

        mArgumentTypes.shrink_to_fit();

        for (const auto& arg: mArgumentTypes) {
            mNeededLocalsForArgs += arg.getSlotCount();
        }

        if (isNative()) {
            return;
        }

        u16 index = 0;

        if (moduleweb_attribute_array_get(&info->attributes, "Code", module->getInfo(), &index)) {
            //TODO: error proper
            std::cout << "Bad function data. No 'Code' attribute defined\n";
            std::exit(1);
        }

        u16 index2 = index + 1;

        if (!moduleweb_attribute_array_get(&info->attributes, "Code", module->getInfo(), &index2)) {
            std::cout << "Bad function data: More than one 'Code' attribute defined\n";
            std::exit(1);
        }

        moduleweb_attribute_info* code = &info->attributes.array[index];

        moduleweb_instream stream;
        if (moduleweb_instream_open_buffer(&stream, code->info, code->length, false)) {
            // TODO: linkage error
            std::cout << "error: failed to stream Code attribute data\n";
            std::exit(1);
        }

        if (moduleweb_code_attribute_init(&mCodeAttribute, &stream)) {
            // TODO: linkage error
            std::cout << "error: failed to read Code attribute data\n";
            std::exit(1);
        }

        moduleweb_instream_close_buffer(&stream);
	}

    Function::~Function() {
        moduleweb_code_attribute_uninit(&mCodeAttribute);
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

    const std::vector<TypeInfo>& Function::getArgumentTypes() const {
        return mArgumentTypes;
    }

    u16 Function::getNeededLocalsForArgs() const {
        return mNeededLocalsForArgs;
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
		return mCodeAttribute.max_locals;
	}
	
	u16 Function::getStackSize() const {
		return mCodeAttribute.max_stack_size;
	}
	
	u8* Function::getEntry() const {
		return mCodeAttribute.code;
	}
	
	u32 Function::getBytecodeSize() const {
		return mCodeAttribute.code_length;
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

    VMContext Function::getNativeContext() {
        return GetContext();
    }
}