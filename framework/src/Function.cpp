#include "JesusVM/Function.h"
#include "JesusVM/JesusVM.h"

#include "JesusVM/bytecode/Executor.h"

#include "JesusVM/constpool/ConstantAscii.h"
#include "JesusVM/constpool/ConstantName.h"

#include "JesusVM/heap/gc/References.h"

#include "moduleweb/module_info.h"

#include <dyncall.h>

#include <cstring>

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

            Stack::Frame* frame = executor.getFrame();

            DCCallVM* callVM = dcNewCallVM((mArgumentTypes.size() + 8) * 8); // it's +8 because we have ctx, but it's also nice to have some extra rooms for anything freaky
            if (callVM == nullptr) {
                std::cout << "Failed to initialize Call Site for native function " << mName << "\n";
                std::exit(1);
            }

#ifdef PLATFORM_WINDOWS
            dcMode(callVM, DC_CALL_C_X64_WIN64);
#else
            dcMode(callVM, DC_CALL_C_X64_SYSV);
#endif

            dcReset(callVM);

            // cache friendly for only double the memory usage
            struct Arg {
                Type type;
                JValue value;
            };

            std::vector<Arg> arguments;
            arguments.reserve(mArgumentTypes.size());

            VMErr err = GC::PushLocalFrame(mArgumentTypes.size()); // would it be more efficient to count reference arguments?
            if (err != VM_OK) {
                std::cout << "Got non-ok error code when preparing reference frame for local function " << mName << " (" << err << ")";
                std::exit(err);
            }

            for (size_t i = 0; i < mArgumentTypes.size(); i++) {
                TypeInfo& arg = mArgumentTypes[mArgumentTypes.size() - 1 - i];

                Arg argument;
                argument.type = arg.getInternalType();

                switch (argument.type) {
                    case Type::REFERENCE: {
                        ObjectRef object = frame->popObject();
                        argument.value.R = object;
                        GC::CreateLocalReference(object.get());
                        object.releaseNoDeref();
                        break;
                    }
                    case Type::HANDLE:
                        argument.value.H = frame->popHandle();
                        break;
                    case Type::BYTE:
                        argument.value.B = static_cast<Byte>(frame->pop());
                        break;
                    case Type::SHORT:
                        argument.value.S = static_cast<Short>(frame->pop());
                        break;
                    case Type::INT:
                        argument.value.I = static_cast<Int>(frame->pop());
                        break;
                    case Type::LONG:
                        argument.value.L = static_cast<Long>(frame->pop());
                        break;
                    case Type::CHAR:
                        argument.value.C = static_cast<Char>(frame->pop());
                        break;
                    case Type::BOOL:
                        argument.value.Z = static_cast<Bool>(frame->pop());
                        break;
                    case Type::FLOAT:
                    case Type::DOUBLE:
                    default:
                        std::cout << "Unhandled type\n";
                        std::exit(1);
                }

                arguments.push_back(argument);
            }

            dcArgPointer(callVM, GetContext());

            for (const Arg& arg : arguments | std::views::reverse) {
                switch (arg.type) {
                    case Type::REFERENCE:
                        dcArgPointer(callVM, arg.value.R);
                        break;
                    case Type::HANDLE:
                        dcArgPointer(callVM, arg.value.H);
                        break;
                    case Type::BYTE:
                        dcArgChar(callVM, arg.value.B);
                        break;
                    case Type::SHORT:
                        dcArgShort(callVM, arg.value.S);
                        break;
                    case Type::INT:
                        dcArgInt(callVM, arg.value.I);
                        break;
                    case Type::LONG:
                        dcArgLongLong(callVM, arg.value.L);
                        break;
                    case Type::CHAR:
                        dcArgChar(callVM, arg.value.C);
                        break;
                    case Type::FLOAT:
                        dcArgFloat(callVM, arg.value.F);
                        break;
                    case Type::DOUBLE:
                        dcArgDouble(callVM, arg.value.D);
                        break;
                    case Type::BOOL:
                        dcArgBool(callVM, arg.value.Z);
                        break;
                }
            }

            switch (mReturnType.getInternalType()) {
                case Type::VOID:
                    dcCallVoid(callVM, getEntry());
                    break;
                case Type::REFERENCE: {
                    Object* object = static_cast<Object*>(dcCallPointer(callVM, getEntry()));
                    frame->pushObject(object);
                    break;
                }
                case Type::HANDLE: {
                    Handle value = dcCallPointer(callVM, getEntry());
                    frame->pushHandle(value);
                    break;
                }
                case Type::BYTE: {
                    Byte value = dcCallChar(callVM, getEntry());
                    frame->push(value);
                    break;
                }
                case Type::SHORT: {
                    Short value = dcCallShort(callVM, getEntry());
                    frame->push(value);
                    break;
                }
                case Type::INT: {
                    Int value = dcCallInt(callVM, getEntry());
                    frame->push(value);
                    break;
                }
                case Type::LONG: {
                    Long value = dcCallLongLong(callVM, getEntry());
                    frame->push(value);
                    break;
                }
                case Type::CHAR: {
                    Char value = dcCallChar(callVM, getEntry());
                    frame->push(value);
                    break;
                }
                case Type::FLOAT:
                case Type::DOUBLE:
                    std::cout << "Unhandled type\n";
                    std::exit(1);
                case Type::BOOL: {
                    Bool value = dcCallBool(callVM, getEntry());
                    frame->push(value);
                    break;
                }
            }

            dcFree(callVM);

            err = GC::PopLocalFrame();
            if (err != VM_OK) {
                std::cout << "Got non-ok error code when finalizing reference frame for local function " << mName << " (" << err << ")";
                std::exit(err);
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

                    i -= 1;
                    frame->setLocalObject(i, obj);

                    break;
                }

                case Type::HANDLE: {
                    auto value = oldFrame->popHandle();

                    i -= 1;
                    frame->setLocalHandle(i, value);

                    break;
                }

                case Type::BYTE: {
                    auto value = oldFrame->pop();

                    i -= 1;
                    frame->setLocal(i, static_cast<Byte>(value));

                    break;
                }

                case Type::SHORT: {
                    auto value = oldFrame->pop();

                    i -= 1;
                    frame->setLocal(i, static_cast<Short>(value));

                    break;
                }

                case Type::INT: {
                    auto value = oldFrame->pop();

                    i -= 1;
                    frame->setLocal(i, static_cast<Int>(value));

                    break;
                }

                case Type::LONG: {
                    auto value = oldFrame->pop();

                    i -= 1;
                    frame->setLocal(i, static_cast<Long>(value));

                    break;
                }

                case Type::CHAR: {
                    auto value = oldFrame->pop();

                    i -= 1;
                    frame->setLocal(i, static_cast<Char>(value));

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

    VMContext* Function::getNativeContext() {
        return GetContext();
    }
}
