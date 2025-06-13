#include "JesusVM/Function.h"
#include "JesusVM/JesusVM.h"

#include "JesusVM/bytecode/Executor.h"

#include "JesusVM/constpool/ConstantAscii.h"
#include "JesusVM/constpool/ConstantName.h"

#include "moduleweb/module_info.h"

#undef BOOL
#undef VOID

namespace JesusVM {
	Function::Function(Module* module, moduleweb_function_info* info)
		: mInfo(info)
        , mModule(module) {
        auto name = module->getConstPool().get<ConstantName>(info->name_index);

        mName = name->getName();
        mDescriptor = name->getDescriptor();

        if (ParseFunctionType(this)) {
            std::cout << "Bad function descriptor\n";
            std::exit(1);
        }

        mArgumentTypes.shrink_to_fit();

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
        return mArgumentTypes.size();
    }

	std::string_view Function::getName() const {
		return mName;
	}
	
	std::string_view Function::getDescriptor() const {
		return mDescriptor;
	}
	
	Function::Modifiers Function::getModifiers() const {
		return mInfo->modifiers;
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
        return (getModifiers() & MODULEWEB_FUNCTION_MODIFIER_PUBLIC) != 0;
    }

    bool Function::isPrivate() const {
        return (getModifiers() & MODULEWEB_FUNCTION_MODIFIER_PRIVATE) != 0;
    }

    bool Function::isPure() const {
        return (getModifiers() & MODULEWEB_FUNCTION_MODIFIER_PURE) != 0;
    }

    bool Function::isAsync() const {
        return (getModifiers() & MODULEWEB_FUNCTION_MODIFIER_ASYNC) != 0;
    }

    bool Function::isNative() const {
        return (getModifiers() & MODULEWEB_FUNCTION_MODIFIER_NATIVE) != 0;
    }

    void Function::call(Executor& executor) {
        if (isAsync()) {
            Threading::Schedule(this);
            return;
        }

        if (isNative()) {
            if (getEntry() == nullptr) {
                Linker::LinkNativeFunction(this);
            }

            std::unique_ptr <JValue[]> nativeArgs = std::make_unique <JValue[]>(getArgumentTypes().size());
            u16 i = getArgumentTypes().size();
            Stack::Frame* frame = executor.getFrame();

            for (const auto& arg: std::ranges::reverse_view(getArgumentTypes())) {
                switch (arg.getInternalType()) {
                    case Type::REFERENCE: {
                        ObjectRef obj = frame->popObject();

                        i -= 1;
                        nativeArgs[i].R = obj;
                        obj->addReference(); // the native arg needs its own explicit reference here

                        break;
                    }

                    case Type::HANDLE: {
                        auto value = frame->popHandle();

                        i -= 1;
                        nativeArgs[i].H = value;

                        break;
                    }

                    case Type::BYTE: {
                        auto value = frame->pop();

                        i -= 1;
                        nativeArgs[i].B = static_cast <Byte>(value);

                        break;
                    }

                    case Type::SHORT: {
                        auto value = frame->pop();

                        i -= 1;
                        nativeArgs[i].S = static_cast <Short>(value);

                        break;
                    }

                    case Type::INT: {
                        auto value = frame->pop();

                        i -= 1;
                        nativeArgs[i].I = static_cast <Int>(value);

                        break;
                    }

                    case Type::LONG: {
                        auto value = frame->pop();

                        i -= 1;
                        nativeArgs[i].L = static_cast <Long>(value);

                        break;
                    }

                    case Type::CHAR: {
                        auto value = frame->pop();

                        i -= 1;
                        nativeArgs[i].C = static_cast <Char>(value);

                        break;
                    }

                    case Type::FLOAT:
                    case Type::DOUBLE:
                        std::exit(3);
                    case Type::BOOL: {
                        auto value = frame->pop();

                        i -= 1;
                        nativeArgs[i].Z = (value != 0);

                        break;
                    }
                }
            }

            if (getReturnType().getInternalType() == Type::VOID) {
                auto ptr = reinterpret_cast <NativeFunctionPtr <void>>(getEntry());
                ptr(GetContext(), nativeArgs.get());
            } else {
                switch (getReturnType().getInternalType()) {
                    case Type::REFERENCE: {
                        auto ptr = reinterpret_cast <NativeFunctionPtr <JObject>>(getEntry());
                        auto value = ptr(GetContext(), nativeArgs.get());
                        frame->pushObject(reinterpret_cast <Object*>(value));
                        break;
                    }

                    case Type::HANDLE: {
                        auto ptr = reinterpret_cast <NativeFunctionPtr <Handle>>(getEntry());
                        auto value = ptr(GetContext(), nativeArgs.get());
                        frame->pushHandle(value);
                        break;
                    }

                    case Type::BYTE: {
                        auto ptr = reinterpret_cast <NativeFunctionPtr <Byte>>(getEntry());
                        auto value = ptr(GetContext(), nativeArgs.get());
                        frame->push(value);
                        break;
                    }

                    case Type::SHORT: {
                        auto ptr = reinterpret_cast <NativeFunctionPtr <Short>>(getEntry());
                        auto value = ptr(GetContext(), nativeArgs.get());
                        frame->push(value);
                        break;
                    }

                    case Type::INT: {
                        auto ptr = reinterpret_cast <NativeFunctionPtr <Int>>(getEntry());
                        auto value = ptr(GetContext(), nativeArgs.get());
                        frame->push(value);
                        break;
                    }

                    case Type::LONG: {
                        auto ptr = reinterpret_cast <NativeFunctionPtr <Long>>(getEntry());
                        auto value = ptr(GetContext(), nativeArgs.get());
                        frame->push(value);
                        break;
                    }

                    case Type::CHAR: {
                        auto ptr = reinterpret_cast <NativeFunctionPtr <Char>>(getEntry());
                        auto value = ptr(GetContext(), nativeArgs.get());
                        frame->push(value);
                        break;
                    }

                    case Type::FLOAT:
                    case Type::DOUBLE:
                        std::exit(3);

                    case Type::BOOL:
                        auto ptr = reinterpret_cast <NativeFunctionPtr <Bool>>(getEntry());
                        auto value = ptr(GetContext(), nativeArgs.get());
                        frame->push(value);
                        break;
                }
            }

            for (i = 0; i < getArgumentTypes().size(); i++) {
                if (getArgumentTypes()[i].getInternalType() == Type::REFERENCE) {
                    auto object = reinterpret_cast <Object*>(nativeArgs[i].R);
                    object->removeReference(); // explicitly remove the native reference after call
                }
            }

            return;
        }

        Stack::Frame* oldFrame = executor.getFrame();
        executor.enterFunction(this);

        Stack::Frame* frame = executor.getFrame();

        u16 i = getNeededLocalsForArgs();


        for (auto& arg: std::ranges::reverse_view(getArgumentTypes())) {
            switch (arg.getInternalType()) {
                case Type::REFERENCE: {
                    ObjectRef obj = oldFrame->popObject();

                    i -= 2;
                    frame->setLocalObject(i, obj);

                    break;
                }

                case Type::HANDLE: {
                    auto value = oldFrame->popHandle();

                    i -= 2;
                    frame->setLocalHandle(i, value);

                    break;
                }

                case Type::BYTE: {
                    auto value = oldFrame->pop();

                    i -= 1;
                    frame->setLocal(i, static_cast <Byte>(value));

                    break;
                }

                case Type::SHORT: {
                    auto value = oldFrame->pop();

                    i -= 1;
                    frame->setLocal(i, static_cast <Short>(value));

                    break;
                }

                case Type::INT: {
                    auto value = oldFrame->pop();

                    i -= 1;
                    frame->setLocal(i, static_cast <Int>(value));

                    break;
                }

                case Type::LONG: {
                    auto value = oldFrame->pop();

                    i -= 2;
                    frame->setLocal(i, static_cast <Long>(value));

                    break;
                }

                case Type::CHAR: {
                    auto value = oldFrame->pop();

                    i -= 1;
                    frame->setLocal(i, static_cast <Char>(value));

                    break;
                }

                case Type::FLOAT:
                case Type::DOUBLE:
                    std::exit(3);
                case Type::BOOL: {
                    auto value = oldFrame->pop();

                    i -= 1;
                    frame->setLocal(i, value != 0);

                    break;
                }
            }
        }
    }

    VMContext Function::getNativeContext() {
        return GetContext();
    }
}