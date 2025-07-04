// Copyright 2025 JesusTouchMe

#include "JesusVM/Linker.h"

#include "JesusVM/heap/gc/References.h"

#include "JesusVM/native_interface/ProvidedFunctions.h"

#include "JesusVM/runtime/std/Primitives.h"

#include "JesusVM/util/StringPool.h"

#include <cstring>

#undef BOOL
#undef VOID

namespace JesusVM::NativeInterface {
    struct VaList {
        va_list args;

        ~VaList() {
            va_end(args);
        }
    };

    void Prepare(_JesusVMNativeInterface* iface) {
        iface->GetModule = GetModule;
        iface->CreateModuleA = CreateModuleA;
        iface->CreateModuleU = CreateModuleU;
        iface->CreateModuleMem = CreateModuleMem;
        iface->Trap = Trap;
        iface->AddTrapHook = AddTrapHook;
        iface->CreateGlobalRef = CreateGlobalRef;
        iface->DeleteGlobalRef = DeleteGlobalRef;
        iface->PushLocalFrame = PushLocalFrame;
        iface->PopLocalFrame = PopLocalFrame;
        iface->CreateLocalRef = CreateLocalRef;
        iface->DeleteLocalRef = DeleteLocalRef;
        iface->GetGlobalVar = GetGlobalVar;
        iface->GetBoolGlobalVar = GetBoolGlobalVar;
        iface->GetByteGlobalVar = GetByteGlobalVar;
        iface->GetShortGlobalVar = GetShortGlobalVar;
        iface->GetIntGlobalVar = GetIntGlobalVar;
        iface->GetLongGlobalVar = GetLongGlobalVar;
        iface->GetCharGlobalVar = GetCharGlobalVar;
        iface->GetFloatGlobalVar = GetFloatGlobalVar;
        iface->GetDoubleGlobalVar = GetDoubleGlobalVar;
        iface->GetObjectGlobalVar = GetObjectGlobalVar;
        iface->GetHandleGlobalVar = GetHandleGlobalVar;
        iface->SetBoolGlobalVar = SetBoolGlobalVar;
        iface->SetByteGlobalVar = SetByteGlobalVar;
        iface->SetShortGlobalVar = SetShortGlobalVar;
        iface->SetIntGlobalVar = SetIntGlobalVar;
        iface->SetLongGlobalVar = SetLongGlobalVar;
        iface->SetCharGlobalVar = SetCharGlobalVar;
        iface->SetFloatGlobalVar = SetFloatGlobalVar;
        iface->SetDoubleGlobalVar = SetDoubleGlobalVar;
        iface->SetObjectGlobalVar = SetObjectGlobalVar;
        iface->SetHandleGlobalVar = SetHandleGlobalVar;
        iface->GetFunction = GetFunction;
        iface->CallVoidFunction = CallVoidFunction;
        iface->CallVoidFunctionA = CallVoidFunctionA;
        iface->CallVoidFunctionV = CallVoidFunctionV;
        iface->CallBoolFunction = CallBoolFunction;
        iface->CallBoolFunctionA = CallBoolFunctionA;
        iface->CallBoolFunctionV = CallBoolFunctionV;
        iface->CallByteFunction = CallByteFunction;
        iface->CallByteFunctionA = CallByteFunctionA;
        iface->CallByteFunctionV = CallByteFunctionV;
        iface->CallShortFunction = CallShortFunction;
        iface->CallShortFunctionA = CallShortFunctionA;
        iface->CallShortFunctionV = CallShortFunctionV;
        iface->CallIntFunction = CallIntFunction;
        iface->CallIntFunctionA = CallIntFunctionA;
        iface->CallIntFunctionV = CallIntFunctionV;
        iface->CallLongFunction = CallLongFunction;
        iface->CallLongFunctionA = CallLongFunctionA;
        iface->CallLongFunctionV = CallLongFunctionV;
        iface->CallCharFunction = CallCharFunction;
        iface->CallCharFunctionA = CallCharFunctionA;
        iface->CallCharFunctionV = CallCharFunctionV;
        iface->CallFloatFunction = CallFloatFunction;
        iface->CallFloatFunctionA = CallFloatFunctionA;
        iface->CallFloatFunctionV = CallFloatFunctionV;
        iface->CallDoubleFunction = CallDoubleFunction;
        iface->CallDoubleFunctionA = CallDoubleFunctionA;
        iface->CallDoubleFunctionV = CallDoubleFunctionV;
        iface->CallObjectFunction = CallObjectFunction;
        iface->CallObjectFunctionA = CallObjectFunctionA;
        iface->CallObjectFunctionV = CallObjectFunctionV;
        iface->CallHandleFunction = CallHandleFunction;
        iface->CallHandleFunctionA = CallHandleFunctionA;
        iface->CallHandleFunctionV = CallHandleFunctionV;
        iface->GetClass = GetClass;
        iface->GetSuperClass = GetSuperClass;
        iface->GetClassName = GetClassName;
        iface->GetClassLinker = GetClassLinker;
        iface->IsAssignable = IsAssignable;
        iface->CreateArtificialClass = CreateArtificialClass;
        iface->AllocObject = AllocObject;
        iface->NewObject = NewObject;
        iface->NewObjectA = NewObjectA;
        iface->NewObjectV = NewObjectV;
        iface->GetObjectClass = GetObjectClass;
        iface->IsInstance = IsInstance;
        iface->GetField = GetField;
        iface->GetBoolField = GetBoolField;
        iface->GetByteField = GetByteField;
        iface->GetShortField = GetShortField;
        iface->GetIntField = GetIntField;
        iface->GetLongField = GetLongField;
        iface->GetCharField = GetCharField;
        iface->GetFloatField = GetFloatField;
        iface->GetDoubleField = GetDoubleField;
        iface->GetObjectField = GetObjectField;
        iface->GetHandleField = GetHandleField;
        iface->SetBoolField = SetBoolField;
        iface->SetByteField = SetByteField;
        iface->SetShortField = SetShortField;
        iface->SetIntField = SetIntField;
        iface->SetLongField = SetLongField;
        iface->SetCharField = SetCharField;
        iface->SetFloatField = SetFloatField;
        iface->SetDoubleField = SetDoubleField;
        iface->SetObjectField = SetObjectField;
        iface->SetHandleField = SetHandleField;
        iface->GetMethod = GetMethod;
        iface->DispatchMethod = DispatchMethod;
        iface->CallVoidMethod = CallVoidMethod;
        iface->CallVoidMethodA = CallVoidMethodA;
        iface->CallVoidMethodV = CallVoidMethodV;
        iface->CallBoolMethod = CallBoolMethod;
        iface->CallBoolMethodA = CallBoolMethodA;
        iface->CallBoolMethodV = CallBoolMethodV;
        iface->CallByteMethod = CallByteMethod;
        iface->CallByteMethodA = CallByteMethodA;
        iface->CallByteMethodV = CallByteMethodV;
        iface->CallShortMethod = CallShortMethod;
        iface->CallShortMethodA = CallShortMethodA;
        iface->CallShortMethodV = CallShortMethodV;
        iface->CallIntMethod = CallIntMethod;
        iface->CallIntMethodA = CallIntMethodA;
        iface->CallIntMethodV = CallIntMethodV;
        iface->CallLongMethod = CallLongMethod;
        iface->CallLongMethodA = CallLongMethodA;
        iface->CallLongMethodV = CallLongMethodV;
        iface->CallCharMethod = CallCharMethod;
        iface->CallCharMethodA = CallCharMethodA;
        iface->CallCharMethodV = CallCharMethodV;
        iface->CallFloatMethod = CallFloatMethod;
        iface->CallFloatMethodA = CallFloatMethodA;
        iface->CallFloatMethodV = CallFloatMethodV;
        iface->CallDoubleMethod = CallDoubleMethod;
        iface->CallDoubleMethodA = CallDoubleMethodA;
        iface->CallDoubleMethodV = CallDoubleMethodV;
        iface->CallObjectMethod = CallObjectMethod;
        iface->CallObjectMethodA = CallObjectMethodA;
        iface->CallObjectMethodV = CallObjectMethodV;
        iface->CallHandleMethod = CallHandleMethod;
        iface->CallHandleMethodA = CallHandleMethodA;
        iface->CallHandleMethodV = CallHandleMethodV;
        iface->NewString = NewString;
        iface->GetStringLength = GetStringLength;
        iface->GetStringChars = GetStringChars;
        iface->ReleaseStringChars = ReleaseStringChars;
        iface->GetArrayLength = GetArrayLength;
        iface->NewObjectArray = NewObjectArray;
        iface->GetObjectArrayElement = GetObjectArrayElement;
        iface->SetObjectArrayElement = SetObjectArrayElement;
        iface->NewBoolArray = NewBoolArray;
        iface->NewByteArray = NewByteArray;
        iface->NewShortArray = NewShortArray;
        iface->NewIntArray = NewIntArray;
        iface->NewLongArray = NewLongArray;
        iface->NewCharArray = NewCharArray;
        iface->NewFloatArray = NewFloatArray;
        iface->NewDoubleArray = NewDoubleArray;
        iface->NewHandleArray = NewHandleArray;
        iface->GetBoolArrayElements = GetBoolArrayElements;
        iface->GetByteArrayElements = GetByteArrayElements;
        iface->GetShortArrayElements = GetShortArrayElements;
        iface->GetIntArrayElements = GetIntArrayElements;
        iface->GetLongArrayElements = GetLongArrayElements;
        iface->GetCharArrayElements = GetCharArrayElements;
        iface->GetFloatArrayElements = GetFloatArrayElements;
        iface->GetDoubleArrayElements = GetDoubleArrayElements;
        iface->GetHandleArrayElements = GetHandleArrayElements;
        iface->ReleaseBoolArrayElements = ReleaseBoolArrayElements;
        iface->ReleaseByteArrayElements = ReleaseByteArrayElements;
        iface->ReleaseShortArrayElements = ReleaseShortArrayElements;
        iface->ReleaseIntArrayElements = ReleaseIntArrayElements;
        iface->ReleaseLongArrayElements = ReleaseLongArrayElements;
        iface->ReleaseCharArrayElements = ReleaseCharArrayElements;
        iface->ReleaseFloatArrayElements = ReleaseFloatArrayElements;
        iface->ReleaseDoubleArrayElements = ReleaseDoubleArrayElements;
        iface->ReleaseHandleArrayElements = ReleaseHandleArrayElements;
    }

    // Module operations

    ModuleRef GetModule(VMContext* ctx, const char* name) {
        return reinterpret_cast<ModuleRef>(Linker::LoadModule(nullptr, name));
    }

    static ModuleRef CreateModuleFromStream(moduleweb_instream* stream, Object* linker, const char* name, Int mode) {
        auto moduleInfo = static_cast<moduleweb_module_info*>(std::malloc(sizeof(moduleweb_module_info)));
        if (moduleInfo == nullptr) {
            return nullptr;
        }

        if (moduleweb_module_info_init(moduleInfo, name, strlen(name), stream)) {
            std::free(moduleInfo);
            return nullptr;
        }

        auto module = new Module(linker, moduleInfo);
        Linker::LinkModule(module);

        switch (mode) {
            default:
                std::cout << "warning: unknown mode was passed to CreateModule in native interface. Defaulted to 0\n";
            case 0: // default mode. release module on reference frame pop
                GC::CreateLocalSpecial(module, [](void* self) {
                    delete static_cast<Module*>(self);
                });
                return reinterpret_cast<ModuleRef>(module);

            case VM_RETAIN_MODULE: // the module will stay isolated
                GC::CreateGlobalSpecial(module, [](void* self) {
                    delete static_cast<Module*>(self);
                });
                return reinterpret_cast<ModuleRef>(module);

            case VM_LINK_GLOBAL: // the module will be put in the linker just like other modules
                return reinterpret_cast<ModuleRef>(Linker::AddExistingModule(module));
        }
    }

    ModuleRef CreateModuleA(VMContext* ctx, const char* name, const char* modulePath, Int mode) {
        moduleweb_instream stream;
        if (moduleweb_instream_open(&stream, modulePath)) {
            return nullptr;
        }

        ModuleRef module = CreateModuleFromStream(&stream, nullptr, name, mode);

        moduleweb_instream_close(&stream);
        return module;
    }

    ModuleRef CreateModuleU(VMContext* ctx, const char* name, const char* modulePath, Int mode) {
        moduleweb_instream stream;
        if (moduleweb_instream_open_utf8(&stream, modulePath)) {
            return nullptr;
        }

        ModuleRef module = CreateModuleFromStream(&stream, nullptr, name, mode);

        moduleweb_instream_close(&stream);
        return module;
    }

    ModuleRef CreateModuleMem(VMContext* ctx, const char* name, JObject linker, const Byte* data, Long dataLength, Int mode) {
        moduleweb_instream stream;
        if (moduleweb_instream_open_buffer(&stream, reinterpret_cast<const u8*>(data), dataLength, false)) {
            return nullptr;
        }

        ModuleRef module = CreateModuleFromStream(&stream, reinterpret_cast<Object*>(linker), name, mode);

        moduleweb_instream_close_buffer(&stream);
        return module;
    }

    // Error operations

    JESUSVM_NORETURN void Trap(VMContext* ctx, const char* message) {
        std::cout << "trap not implemented!\n";
    }

    void AddTrapHook(VMContext* ctx, void hook(VMContext* ctx, TrapInfo* info)) {
        std::cout << "trap hooks not implemented!\n";
    }

    // References

    Handle CreateGlobalRef(VMContext* ctx, JObject obj) {
        return GC::CreateGlobalReference(reinterpret_cast<Object*>(obj));
    }

    void DeleteGlobalRef(VMContext* ctx, Handle ref) {
        GC::DeleteGlobalReference(ref);
    }

    VMErr PushLocalFrame(VMContext* ctx, Int initial_capacity) {
        return GC::PushLocalFrame(initial_capacity);
    }

    VMErr PopLocalFrame(VMContext* ctx) {
        return GC::PopLocalFrame();
    }

    Handle CreateLocalRef(VMContext* ctx, JObject obj) {
        return GC::CreateLocalReference(reinterpret_cast<Object*>(obj));
    }

    void DeleteLocalRef(VMContext* ctx, Handle ref) {
        GC::DeleteLocalReference(ref);
    }

    // Global var operations

    GlobalVarRef GetGlobalVar(VMContext* ctx, ModuleRef moduleRef, const char* name, const char* descriptor) {
        Module* module = reinterpret_cast<Module*>(moduleRef);
        GlobalVar* global = module->getGlobalVar(name, descriptor);

        return reinterpret_cast<GlobalVarRef>(global);
    }

    Bool GetBoolGlobalVar(VMContext* ctx, GlobalVarRef globalRef) {
        GlobalVar* global = reinterpret_cast<GlobalVar*>(globalRef);
        return global->getValue().Z;
    }

    Byte GetByteGlobalVar(VMContext* ctx, GlobalVarRef globalRef) {
        GlobalVar* global = reinterpret_cast<GlobalVar*>(globalRef);
        return global->getValue().B;
    }

    Short GetShortGlobalVar(VMContext* ctx, GlobalVarRef globalRef) {
        GlobalVar* global = reinterpret_cast<GlobalVar*>(globalRef);
        return global->getValue().S;
    }

    Int GetIntGlobalVar(VMContext* ctx, GlobalVarRef globalRef) {
        GlobalVar* global = reinterpret_cast<GlobalVar*>(globalRef);
        return global->getValue().I;
    }

    Long GetLongGlobalVar(VMContext* ctx, GlobalVarRef globalRef) {
        GlobalVar* global = reinterpret_cast<GlobalVar*>(globalRef);
        return global->getValue().L;
    }

    Char GetCharGlobalVar(VMContext* ctx, GlobalVarRef globalRef) {
        GlobalVar* global = reinterpret_cast<GlobalVar*>(globalRef);
        return global->getValue().C;
    }

    Float GetFloatGlobalVar(VMContext* ctx, GlobalVarRef globalRef) {
        GlobalVar* global = reinterpret_cast<GlobalVar*>(globalRef);
        return global->getValue().F;
    }

    Double GetDoubleGlobalVar(VMContext* ctx, GlobalVarRef globalRef) {
        GlobalVar* global = reinterpret_cast<GlobalVar*>(globalRef);
        return global->getValue().D;
    }

    JObject GetObjectGlobalVar(VMContext* ctx, GlobalVarRef globalRef) {
        GlobalVar* global = reinterpret_cast<GlobalVar*>(globalRef);
        return global->getValue().R;
    }

    Handle GetHandleGlobalVar(VMContext* ctx, GlobalVarRef globalRef) {
        GlobalVar* global = reinterpret_cast<GlobalVar*>(globalRef);
        return global->getValue().H;
    }

    Bool SetBoolGlobalVar(VMContext* ctx, GlobalVarRef globalRef, Bool newValue) {
        GlobalVar* global = reinterpret_cast<GlobalVar*>(globalRef);
        auto old = global->getValue().Z;
        global->setValue(newValue);
        return old;
    }

    Byte SetByteGlobalVar(VMContext* ctx, GlobalVarRef globalRef, Byte newValue) {
        GlobalVar* global = reinterpret_cast<GlobalVar*>(globalRef);
        auto old = global->getValue().B;
        global->setValue(newValue);
        return old;
    }

    Short SetShortGlobalVar(VMContext* ctx, GlobalVarRef globalRef, Short newValue) {
        GlobalVar* global = reinterpret_cast<GlobalVar*>(globalRef);
        auto old = global->getValue().S;
        global->setValue(newValue);
        return old;
    }

    Int SetIntGlobalVar(VMContext* ctx, GlobalVarRef globalRef, Int newValue) {
        GlobalVar* global = reinterpret_cast<GlobalVar*>(globalRef);
        auto old = global->getValue().I;
        global->setValue(newValue);
        return old;
    }

    Long SetLongGlobalVar(VMContext* ctx, GlobalVarRef globalRef, Long newValue) {
        GlobalVar* global = reinterpret_cast<GlobalVar*>(globalRef);
        auto old = global->getValue().L;
        global->setValue(newValue);
        return old;
    }

    Char SetCharGlobalVar(VMContext* ctx, GlobalVarRef globalRef, Char newValue) {
        GlobalVar* global = reinterpret_cast<GlobalVar*>(globalRef);
        auto old = global->getValue().C;
        global->setValue(newValue);
        return old;
    }

    Float SetFloatGlobalVar(VMContext* ctx, GlobalVarRef globalRef, Float newValue) {
        GlobalVar* global = reinterpret_cast<GlobalVar*>(globalRef);
        auto old = global->getValue().F;
        global->setValue(newValue);
        return old;
    }

    Double SetDoubleGlobalVar(VMContext* ctx, GlobalVarRef globalRef, Double newValue) {
        GlobalVar* global = reinterpret_cast<GlobalVar*>(globalRef);
        auto old = global->getValue().D;
        global->setValue(newValue);
        return old;
    }

    JObject SetObjectGlobalVar(VMContext* ctx, GlobalVarRef globalRef, JObject newValue) {
        GlobalVar* global = reinterpret_cast<GlobalVar*>(globalRef);
        auto old = global->getValue().R;
        global->setValue(newValue);
        return old;
    }

    Handle SetHandleGlobalVar(VMContext* ctx, GlobalVarRef globalRef, Handle newValue) {
        GlobalVar* global = reinterpret_cast<GlobalVar*>(globalRef);
        auto old = global->getValue().H;
        global->setValue(newValue);
        return old;
    }

    // Function operations

    FunctionRef GetFunction(VMContext* ctx, ModuleRef moduleRef, const char* name, const char* descriptor) {
        Module* module = reinterpret_cast<Module*>(moduleRef);
        Function* function = module->getFunction(name, descriptor);

        return reinterpret_cast<FunctionRef>(function);
    }

    template<class Ret, Type RetType>
    static Ret CallTFunctionA(VMContext* ctx, FunctionRef functionRef, const JValue* args, JObject thisArgument = nullptr) {
        Function* function = reinterpret_cast<Function*>(functionRef);
        Executor& executor = Threading::CurrentThread::GetExecutor();

        executor.enterFunction(function);

        Stack::Frame* frame = executor.getFrame();

        u16 i = 0;
        u16 argIndex = 0;

        if (thisArgument != nullptr) {
            frame->setLocalObject(i, reinterpret_cast<Object*>(thisArgument));
            i++;
        }

        for (const auto& arg : function->getArgumentTypes()) {
            switch (arg.getInternalType()) {
                case Type::REFERENCE:
                    frame->setLocalObject(i, reinterpret_cast<Object*>(args[argIndex].R));
                    break;
                case Type::HANDLE:
                    frame->setLocalHandle(i, args[argIndex].H);
                    break;
                case Type::BYTE:
                    frame->setLocal(i, args[argIndex].B);
                    break;
                case Type::SHORT:
                    frame->setLocal(i, args[argIndex].S);
                    break;
                case Type::INT:
                    frame->setLocal(i, args[argIndex].I);
                    break;
                case Type::LONG:
                    frame->setLocal(i, args[argIndex].L);
                    break;
                case Type::CHAR:
                    frame->setLocal(i, args[argIndex].C);
                    break;
                case Type::FLOAT:
                case Type::DOUBLE:
                    std::cout << "floating point not implemented\n";
                    std::exit(1);
                case Type::BOOL:
                    frame->setLocal(i, args[argIndex].Z);
                    break;
            }

            i++;
            argIndex++;
        }

        executor.run();

        if constexpr (!std::is_same_v<Ret, void>) {
            return static_cast<Ret>(executor.getReturnValue());
        }
    }

    template<class Ret, Type RetType>
    static Ret CallTFunctionV(VMContext* ctx, FunctionRef functionRef, va_list args, JObject thisArgument = nullptr) {
        Function* function = reinterpret_cast<Function*>(functionRef);
        Executor& executor = Threading::CurrentThread::GetExecutor();

        executor.enterFunction(function);

        Stack::Frame* frame = executor.getFrame();

        u16 i = 0;

        if (thisArgument != nullptr) {
            frame->setLocalObject(i, reinterpret_cast<Object*>(thisArgument));
            i++;
        }

        for (const auto& arg: function->getArgumentTypes()) {
            switch (arg.getInternalType()) {
                case Type::REFERENCE:
                    frame->setLocalObject(i, va_arg(args, Object*));
                    break;
                case Type::HANDLE:
                    frame->setLocalHandle(i, va_arg(args, Handle));
                    break;
                case Type::BYTE:
                    frame->setLocal(i, va_arg(args, Byte));
                    break;
                case Type::SHORT:
                    frame->setLocal(i, va_arg(args, Short));
                    break;
                case Type::INT:
                    frame->setLocal(i, va_arg(args, Int));
                    break;
                case Type::LONG:
                    frame->setLocal(i, va_arg(args, Long));
                    break;
                case Type::CHAR:
                    frame->setLocal(i, va_arg(args, Char));
                    break;
                case Type::FLOAT:
                case Type::DOUBLE:
                    std::cout << "floating point not implemented\n";
                    std::exit(1);
                case Type::BOOL:
                    frame->setLocal(i, va_arg(args, Bool));
                    break;
            }

            i++;
        }

        executor.run();

        if constexpr (!std::is_same_v<Ret, void>) {
            return static_cast<Ret>(executor.getReturnValue());
        }
    }

    void CallVoidFunction(VMContext* ctx, FunctionRef functionRef, ...) {
        VaList args{};
        va_start(args.args, functionRef);
        CallVoidFunctionV(ctx, functionRef, args.args);
    }

    void CallVoidFunctionA(VMContext* ctx, FunctionRef functionRef, const JValue* args) {
        CallTFunctionA<void, Type::VOID>(ctx, functionRef, args);
    }

    void CallVoidFunctionV(VMContext* ctx, FunctionRef functionRef, va_list args) {
        CallTFunctionV<void, Type::VOID>(ctx, functionRef, args);
    }

    Bool CallBoolFunction(VMContext* ctx, FunctionRef functionRef, ...) {
        VaList args{};
        va_start(args.args, functionRef);
        return CallBoolFunctionV(ctx, functionRef, args.args);
    }

    Bool CallBoolFunctionA(VMContext* ctx, FunctionRef functionRef, const JValue* args) {
        return CallTFunctionA<Bool, Type::BOOL>(ctx, functionRef, args);
    }

    Bool CallBoolFunctionV(VMContext* ctx, FunctionRef functionRef, va_list args) {
        return CallTFunctionV<Bool, Type::BOOL>(ctx, functionRef, args);
    }

    Byte CallByteFunction(VMContext* ctx, FunctionRef functionRef, ...) {
        VaList args{};
        va_start(args.args, functionRef);
        return CallByteFunctionV(ctx, functionRef, args.args);
    }

    Byte CallByteFunctionA(VMContext* ctx, FunctionRef functionRef, const JValue* args) {
        return CallTFunctionA<Byte, Type::BYTE>(ctx, functionRef, args);
    }

    Byte CallByteFunctionV(VMContext* ctx, FunctionRef functionRef, va_list args) {
        return CallTFunctionV<Byte, Type::BYTE>(ctx, functionRef, args);
    }

    Short CallShortFunction(VMContext* ctx, FunctionRef functionRef, ...) {
        VaList args{};
        va_start(args.args, functionRef);
        return CallShortFunctionV(ctx, functionRef, args.args);
    }

    Short CallShortFunctionA(VMContext* ctx, FunctionRef functionRef, const JValue* args) {
        return CallTFunctionA<Short, Type::SHORT>(ctx, functionRef, args);
    }

    Short CallShortFunctionV(VMContext* ctx, FunctionRef functionRef, va_list args) {
        return CallTFunctionV<Short, Type::SHORT>(ctx, functionRef, args);
    }

    Int CallIntFunction(VMContext* ctx, FunctionRef functionRef, ...) {
        VaList args{};
        va_start(args.args, functionRef);
        return CallIntFunctionV(ctx, functionRef, args.args);
    }

    Int CallIntFunctionA(VMContext* ctx, FunctionRef functionRef, const JValue* args) {
        return CallTFunctionA<Int, Type::INT>(ctx, functionRef, args);
    }

    Int CallIntFunctionV(VMContext* ctx, FunctionRef functionRef, va_list args) {
        return CallTFunctionV<Int, Type::INT>(ctx, functionRef, args);
    }

    Long CallLongFunction(VMContext* ctx, FunctionRef functionRef, ...) {
        VaList args{};
        va_start(args.args, functionRef);
        return CallLongFunctionV(ctx, functionRef, args.args);
    }

    Long CallLongFunctionA(VMContext* ctx, FunctionRef functionRef, const JValue* args) {
        return CallTFunctionA<Long, Type::LONG>(ctx, functionRef, args);
    }

    Long CallLongFunctionV(VMContext* ctx, FunctionRef functionRef, va_list args) {
        return CallTFunctionV<Long, Type::LONG>(ctx, functionRef, args);
    }

    Char CallCharFunction(VMContext* ctx, FunctionRef functionRef, ...) {
        VaList args{};
        va_start(args.args, functionRef);
        return CallCharFunctionV(ctx, functionRef, args.args);
    }

    Char CallCharFunctionA(VMContext* ctx, FunctionRef functionRef, const JValue* args) {
        return CallTFunctionA<Char, Type::CHAR>(ctx, functionRef, args);
    }

    Char CallCharFunctionV(VMContext* ctx, FunctionRef functionRef, va_list args) {
        return CallTFunctionV<Char, Type::CHAR>(ctx, functionRef, args);
    }

    Float CallFloatFunction(VMContext* ctx, FunctionRef functionRef, ...) {
        VaList args{};
        va_start(args.args, functionRef);
        return CallFloatFunctionV(ctx, functionRef, args.args);
    }

    Float CallFloatFunctionA(VMContext* ctx, FunctionRef functionRef, const JValue* args) {
        return CallTFunctionA<Float, Type::FLOAT>(ctx, functionRef, args);
    }

    Float CallFloatFunctionV(VMContext* ctx, FunctionRef functionRef, va_list args) {
        return CallTFunctionV<Float, Type::FLOAT>(ctx, functionRef, args);
    }

    Double CallDoubleFunction(VMContext* ctx, FunctionRef functionRef, ...) {
        VaList args{};
        va_start(args.args, functionRef);
        return CallDoubleFunctionV(ctx, functionRef, args.args);
    }

    Double CallDoubleFunctionA(VMContext* ctx, FunctionRef functionRef, const JValue* args) {
        return CallTFunctionA<Double, Type::DOUBLE>(ctx, functionRef, args);
    }

    Double CallDoubleFunctionV(VMContext* ctx, FunctionRef functionRef, va_list args) {
        return CallTFunctionV<Double, Type::DOUBLE>(ctx, functionRef, args);
    }

    JObject CallObjectFunction(VMContext* ctx, FunctionRef functionRef, ...) {
        VaList args{};
        va_start(args.args, functionRef);
        return CallObjectFunctionV(ctx, functionRef, args.args);
    }

    JObject CallObjectFunctionA(VMContext* ctx, FunctionRef functionRef, const JValue* args) {
        JObject ret = CallTFunctionA<JObject, Type::REFERENCE>(ctx, functionRef, args);
        CreateLocalRef(ctx, ret);
        return ret;
    }

    JObject CallObjectFunctionV(VMContext* ctx, FunctionRef functionRef, va_list args) {
        JObject ret = CallTFunctionV<JObject, Type::REFERENCE>(ctx, functionRef, args);
        CreateLocalRef(ctx, ret);
        return ret;
    }

    Handle CallHandleFunction(VMContext* ctx, FunctionRef functionRef, ...) {
        VaList args{};
        va_start(args.args, functionRef);
        return CallHandleFunctionV(ctx, functionRef, args.args);
    }

    Handle CallHandleFunctionA(VMContext* ctx, FunctionRef functionRef, const JValue* args) {
        return CallTFunctionA<Handle, Type::HANDLE>(ctx, functionRef, args);
    }

    Handle CallHandleFunctionV(VMContext* ctx, FunctionRef functionRef, va_list args) {
        return CallTFunctionV<Handle, Type::HANDLE>(ctx, functionRef, args);
    }

    // Class operations

    ClassRef GetClass(VMContext* ctx, ModuleRef moduleRef, const char* name) {
        Module* module = reinterpret_cast<Module*>(moduleRef);
        Class* clas = module->getClass(name);

        return reinterpret_cast<ClassRef>(clas);
    }

    ClassRef GetSuperClass(VMContext* ctx, ClassRef classRef) {
        Class* clas = reinterpret_cast<Class*>(classRef);
        return reinterpret_cast<ClassRef>(clas->getSuperClass());
    }

    const char* GetClassName(VMContext* ctx, ClassRef classRef) {
        Class* clas = reinterpret_cast<Class*>(classRef);
        return StringPool::InternCString(clas->getName()); // TODO: maybe allocate this on its own thing
    }

    JObject GetClassLinker(VMContext* ctx, ClassRef classRef) {
        Class* clas = reinterpret_cast<Class*>(classRef);
        return reinterpret_cast<JObject>(clas->getModule()->getLinker());
    }

    Bool IsAssignable(VMContext* ctx, ClassRef fromRef, ClassRef toRef) {
        Class* from = reinterpret_cast<Class*>(fromRef);
        Class* to = reinterpret_cast<Class*>(toRef);

        return from->isAssignableTo(to);
    }

    ClassRef CreateArtificialClass(VMContext* ctx, ModuleRef module, ArtificialClass* clas) {
        std::cout << "creating artificial class not implemented\n";
        return nullptr;
    }

    // Object operations

    JObject AllocObject(VMContext* ctx, ClassRef classRef) {
        Class* clas = reinterpret_cast<Class*>(classRef);
        auto obj = ::JesusVM::AllocObject(clas);

        GC::CreateLocalReference(obj.get());

        return obj;
    }

    JObject NewObject(VMContext* ctx, ClassRef clas, FunctionRef constructor, ...) {
        VaList args{};
        va_start(args.args, constructor);
        return NewObjectV(ctx, clas, constructor, args.args);
    }

    JObject NewObjectA(VMContext* ctx, ClassRef clas, FunctionRef constructor, const JValue* args) {
        JObject self = AllocObject(ctx, clas);
        CallTFunctionA<void, Type::VOID>(ctx, constructor, args, self);
        return self;
    }

    JObject NewObjectV(VMContext* ctx, ClassRef clas, FunctionRef constructor, va_list args) {
        JObject self = AllocObject(ctx, clas);
        CallTFunctionV<void, Type::VOID>(ctx, constructor, args, self);
        return self;
    }

    ClassRef GetObjectClass(VMContext* ctx, JObject obj) {
        Object* object = reinterpret_cast<Object*>(obj);
        return reinterpret_cast<ClassRef>(object->getClass());
    }

    Bool IsInstance(VMContext* ctx, JObject obj, ClassRef classRef) {
        Object* object = reinterpret_cast<Object*>(obj);
        Class* clas = reinterpret_cast<Class*>(classRef);

        return object->isInstance(clas);
    }

    // Field operations

    FieldRef GetField(VMContext* ctx, ClassRef classRef, const char* name, const char* descriptor) {
        Class* clas = reinterpret_cast<Class*>(classRef);
        return reinterpret_cast<FieldRef>(clas->getField(name, descriptor));
    }

    template<class Ret, auto GetMethod>
    static Ret GetTField(VMContext* ctx, JObject obj, FieldRef fieldRef) {
        Object* object = reinterpret_cast<Object*>(obj);
        Field* field = reinterpret_cast<Field*>(fieldRef);

        return (object->*GetMethod)(field);
    }

    template<class Ret, auto GetMethod, auto SetMethod>
    static Ret SetTField(VMContext* ctx, JObject obj, FieldRef fieldRef, Ret value) {
        Object* object = reinterpret_cast<Object*>(obj);
        Field* field = reinterpret_cast<Field*>(fieldRef);

        Ret old = (object->*GetMethod)(field);
        (object->*SetMethod)(field, value);

        return old;
    }

    Bool GetBoolField(VMContext* ctx, JObject obj, FieldRef fieldRef) {
        return GetTField<Bool, &Object::getBool>(ctx, obj, fieldRef);
    }

    Byte GetByteField(VMContext* ctx, JObject obj, FieldRef fieldRef) {
        return GetTField<Byte, &Object::getByte>(ctx, obj, fieldRef);
    }

    Short GetShortField(VMContext* ctx, JObject obj, FieldRef fieldRef) {
        return GetTField<Short, &Object::getShort>(ctx, obj, fieldRef);
    }

    Int GetIntField(VMContext* ctx, JObject obj, FieldRef fieldRef) {
        return GetTField<Int, &Object::getInt>(ctx, obj, fieldRef);
    }

    Long GetLongField(VMContext* ctx, JObject obj, FieldRef fieldRef) {
        return GetTField<Long, &Object::getLong>(ctx, obj, fieldRef);
    }

    Char GetCharField(VMContext* ctx, JObject obj, FieldRef fieldRef) {
        return GetTField<Char, &Object::getChar>(ctx, obj, fieldRef);
    }

    Float GetFloatField(VMContext* ctx, JObject obj, FieldRef fieldRef) {
        std::cout << "float not implemented\n";
        std::exit(1);
    }

    Double GetDoubleField(VMContext* ctx, JObject obj, FieldRef fieldRef) {
        std::cout << "double not implemented\n";
        std::exit(1);
    }

    JObject GetObjectField(VMContext* ctx, JObject obj, FieldRef fieldRef) {
        JObject ret = GetTField<JObject, &Object::getObject>(ctx, obj, fieldRef);
        CreateLocalRef(ctx, ret);
        return ret;
    }

    Handle GetHandleField(VMContext* ctx, JObject obj, FieldRef fieldRef) {
        return GetTField<Handle, &Object::getHandle>(ctx, obj, fieldRef);
    }

    Bool SetBoolField(VMContext* ctx, JObject obj, FieldRef fieldRef, Bool newValue) {
        return SetTField<Bool, &Object::getBool, &Object::setBool>(ctx, obj, fieldRef, newValue);
    }

    Byte SetByteField(VMContext* ctx, JObject obj, FieldRef fieldRef, Byte newValue) {
        return SetTField<Byte, &Object::getByte, &Object::setByte>(ctx, obj, fieldRef, newValue);
    }

    Short SetShortField(VMContext* ctx, JObject obj, FieldRef fieldRef, Short newValue) {
        return SetTField<Short, &Object::getShort, &Object::setShort>(ctx, obj, fieldRef, newValue);
    }

    Int SetIntField(VMContext* ctx, JObject obj, FieldRef fieldRef, Int newValue) {
        return SetTField<Int, &Object::getInt, &Object::setInt>(ctx, obj, fieldRef, newValue);
    }

    Long SetLongField(VMContext* ctx, JObject obj, FieldRef fieldRef, Long newValue) {
        return SetTField<Long, &Object::getLong, &Object::setLong>(ctx, obj, fieldRef, newValue);
    }

    Char SetCharField(VMContext* ctx, JObject obj, FieldRef fieldRef, Char newValue) {
        return SetTField<Char, &Object::getChar, &Object::setChar>(ctx, obj, fieldRef, newValue);
    }

    Float SetFloatField(VMContext* ctx, JObject obj, FieldRef fieldRef, Float newValue) {
        std::cout << "float not implemented\n";
        std::exit(1);
    }

    Double SetDoubleField(VMContext* ctx, JObject obj, FieldRef fieldRef, Double newValue) {
        std::cout << "double not implemented\n";
        std::exit(1);
    }

    JObject SetObjectField(VMContext* ctx, JObject obj, FieldRef fieldRef, JObject newValue) {
        JObject ret = reinterpret_cast<JObject>(SetTField<Object*, &Object::getObject, &Object::setObject>(ctx, obj, fieldRef, reinterpret_cast<Object*>(newValue)));
        CreateLocalRef(ctx, ret);
        return ret;
    }

    Handle SetHandleField(VMContext* ctx, JObject obj, FieldRef fieldRef, Handle newValue) {
        return SetTField<Handle, &Object::getHandle, &Object::setHandle>(ctx, obj, fieldRef, newValue);
    }

    // Method operations

    MethodRef GetMethod(VMContext* ctx, ClassRef classRef, const char* name, const char* descriptor) {
        Class* clas = reinterpret_cast<Class*>(classRef);
        return reinterpret_cast<MethodRef>(clas->getMethod(name, descriptor));
    }

    FunctionRef DispatchMethod(VMContext* ctx, ClassRef classRef, MethodRef methodRef) {
        Class* clas = reinterpret_cast<Class*>(classRef);
        Method* method = reinterpret_cast<Method*>(methodRef);

        return reinterpret_cast<FunctionRef>(clas->dispatchMethod(method));
    }

    void CallVoidMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
        VaList args{};
        va_start(args.args, obj);
        CallVoidMethodV(ctx, method, obj, args.args);
    }

    void CallVoidMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        CallTFunctionA<void, Type::VOID>(ctx, function, args, obj);
    }

    void CallVoidMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        CallTFunctionV<void, Type::VOID>(ctx, function, args, obj);
    }

    Bool CallBoolMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
        VaList args{};
        va_start(args.args, obj);
        return CallBoolMethodV(ctx, method, obj, args.args);
    }

    Bool CallBoolMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        return CallTFunctionA<bool, Type::BOOL>(ctx, function, args, obj);
    }

    Bool CallBoolMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        return CallTFunctionV<bool, Type::BOOL>(ctx, function, args, obj);
    }

    Byte CallByteMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
        VaList args{};
        va_start(args.args, obj);
        return CallByteMethodV(ctx, method, obj, args.args);
    }

    Byte CallByteMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        return CallTFunctionA<Byte, Type::BYTE>(ctx, function, args, obj);
    }

    Byte CallByteMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        return CallTFunctionV<Byte, Type::BYTE>(ctx, function, args, obj);
    }

    Short CallShortMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
        VaList args{};
        va_start(args.args, obj);
        return CallShortMethodV(ctx, method, obj, args.args);
    }

    Short CallShortMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        return CallTFunctionA<Short, Type::SHORT>(ctx, function, args, obj);
    }

    Short CallShortMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        return CallTFunctionV<Short, Type::SHORT>(ctx, function, args, obj);
    }

    Int CallIntMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
        VaList args{};
        va_start(args.args, obj);
        return CallIntMethodV(ctx, method, obj, args.args);
    }

    Int CallIntMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        return CallTFunctionA<Int, Type::INT>(ctx, function, args, obj);
    }

    Int CallIntMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        return CallTFunctionV<Int, Type::INT>(ctx, function, args, obj);
    }

    Long CallLongMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
        VaList args{};
        va_start(args.args, obj);
        return CallLongMethodV(ctx, method, obj, args.args);
    }

    Long CallLongMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        return CallTFunctionA<Long, Type::LONG>(ctx, function, args, obj);
    }

    Long CallLongMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        return CallTFunctionV<Long, Type::LONG>(ctx, function, args, obj);
    }

    Char CallCharMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
        VaList args{};
        va_start(args.args, obj);
        return CallCharMethodV(ctx, method, obj, args.args);
    }

    Char CallCharMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        return CallTFunctionA<Char, Type::CHAR>(ctx, function, args, obj);
    }

    Char CallCharMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        return CallTFunctionV<Char, Type::CHAR>(ctx, function, args, obj);
    }

    Float CallFloatMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
        VaList args{};
        va_start(args.args, obj);
        return CallFloatMethodV(ctx, method, obj, args.args);
    }

    Float CallFloatMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        return CallTFunctionA<Float, Type::FLOAT>(ctx, function, args, obj);
    }

    Float CallFloatMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        return CallTFunctionV<Float, Type::FLOAT>(ctx, function, args, obj);
    }

    Double CallDoubleMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
        VaList args{};
        va_start(args.args, obj);
        return CallDoubleMethodV(ctx, method, obj, args.args);
    }

    Double CallDoubleMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        return CallTFunctionA<Double, Type::DOUBLE>(ctx, function, args, obj);
    }

    Double CallDoubleMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        return CallTFunctionV<Double, Type::DOUBLE>(ctx, function, args, obj);
    }

    JObject CallObjectMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
        VaList args{};
        va_start(args.args, obj);
        return CallObjectMethodV(ctx, method, obj, args.args);
    }

    JObject CallObjectMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        JObject ret = CallTFunctionA<JObject, Type::REFERENCE>(ctx, function, args, obj);
        CreateLocalRef(ctx, ret);
        return ret;
    }

    JObject CallObjectMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        JObject ret = CallTFunctionV<JObject, Type::REFERENCE>(ctx, function, args, obj);
        CreateLocalRef(ctx, ret);
        return ret;
    }

    Handle CallHandleMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
        VaList args{};
        va_start(args.args, obj);
        return CallHandleMethodV(ctx, method, obj, args.args);
    }

    Handle CallHandleMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        return CallTFunctionA<Handle, Type::HANDLE>(ctx, function, args, obj);
    }

    Handle CallHandleMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
        FunctionRef function = DispatchMethod(ctx, GetObjectClass(ctx, obj), method);
        return CallTFunctionV<Handle, Type::HANDLE>(ctx, function, args, obj);
    }

    // Primitives::String class operations

    String NewString(VMContext* ctx, const Char* chars, Long length) {
        ObjectRef array = AllocPrimitiveArray(T_CHAR, length);
        auto data = array->getArrayElements<Char>();

        std::memcpy(data, chars, length);

        ObjectRef obj = ::JesusVM::AllocObject(rt::std::Primitives::String::self);
        obj->setObject(rt::std::Primitives::String::data, array);

        GC::CreateLocalReference(obj);

        return obj;
    }

    Long GetStringLength(VMContext* ctx, String string) {
        Object* obj = reinterpret_cast<Object*>(string);
        Object* data = obj->getObject(rt::std::Primitives::String::data);
        return data->getArrayLength();
    }

    char* GetStringChars(VMContext* ctx, String string) {
        Object* obj = reinterpret_cast<Object*>(string);
        Object* data = obj->getObject(rt::std::Primitives::String::data);
        Char* chars = data->getArrayElements<Char>();

        char* charCopy = static_cast<char*>(std::malloc(data->getArrayLength() * sizeof(Char)));
        std::memcpy(charCopy, chars, data->getArrayLength() * sizeof(Char));

        return charCopy;
    }

    void ReleaseStringChars(VMContext* ctx, char* chars) {
        std::free(chars);
    }

    // Array operations

    Long GetArrayLength(VMContext* ctx, ::Array array) {
        Object* obj = reinterpret_cast<Object*>(array);
        return obj->getArrayLength();
    }

    ObjectArray NewObjectArray(VMContext* ctx, Long length, ClassRef elementClassRef, JObject initialElementRef) {
        Class* elementClass = reinterpret_cast<Class*>(elementClassRef);
        Object* initialElement = reinterpret_cast<Object*>(initialElementRef);

        Object* obj = AllocArrayOf(elementClass, length);
        Object** data = obj->getArrayElements<Object*>();

        for (Long i = 0; i < length; i++) {
            data[i] = initialElement;
        }

        return reinterpret_cast<ObjectArray>(obj);
    }

    JObject GetObjectArrayElement(VMContext* ctx, ObjectArray array, Long index) {
        Object* obj = reinterpret_cast<Object*>(array);
        Object** data = obj->getArrayElements<Object*>();
        //TODO: check index

        return reinterpret_cast<JObject>(data[index]);
    }

    JObject SetObjectArrayElement(VMContext* ctx, ObjectArray array, Long index, JObject valueRef) {
        Object* obj = reinterpret_cast<Object*>(array);
        Object* value = reinterpret_cast<Object*>(valueRef);
        Object** data = obj->getArrayElements<Object*>();
        //TODO: check index

        Object* old = data[index];
        if (old != value) old->removeReference();

        data[index] = value;
        if (value != nullptr) value->addReference();

        GC::CreateLocalReference(old);

        return reinterpret_cast<JObject>(old);
    }

    template<class Ret, u8 TypeId>
    static Ret NewTArray(VMContext* ctx, Long length) {
        ObjectRef obj = AllocPrimitiveArray(TypeId, length);
        GC::CreateLocalReference(obj);
        return reinterpret_cast<Ret>(obj.get());
    }

    BoolArray NewBoolArray(VMContext* ctx, Long length) {
        return NewTArray<BoolArray, T_BOOL>(ctx, length);
    }

    ByteArray NewByteArray(VMContext* ctx, Long length) {
        return NewTArray<ByteArray, T_BYTE>(ctx, length);
    }

    ShortArray NewShortArray(VMContext* ctx, Long length) {
        return NewTArray<ShortArray, T_SHORT>(ctx, length);
    }

    IntArray NewIntArray(VMContext* ctx, Long length) {
        return NewTArray<IntArray, T_INT>(ctx, length);
    }

    LongArray NewLongArray(VMContext* ctx, Long length) {
        return NewTArray<LongArray, T_LONG>(ctx, length);
    }

    CharArray NewCharArray(VMContext* ctx, Long length) {
        return NewTArray<CharArray, T_CHAR>(ctx, length);
    }

    FloatArray NewFloatArray(VMContext* ctx, Long length) {
        return NewTArray<FloatArray, T_FLOAT>(ctx, length);
    }

    DoubleArray NewDoubleArray(VMContext* ctx, Long length) {
        return NewTArray<DoubleArray, T_DOUBLE>(ctx, length);
    }

    HandleArray NewHandleArray(VMContext* ctx, Long length) {
        return NewTArray<HandleArray, T_HANDLE>(ctx, length);
    }

    template<class Data, class Arr>
    Data* GetTArrayElements(VMContext* ctx, Arr array) {
        Object* obj = reinterpret_cast<Object*>(array);
        Long length = obj->getArrayLength();
        Data* elements = obj->getArrayElements<Data>();

        Data* copy = static_cast<Data*>(std::malloc(length * sizeof(Data)));
        if (copy == nullptr) return nullptr;
        std::memcpy(copy, elements, length * sizeof(Data));

        return copy;
    }

    template<class Data, class Arr>
    void ReleaseTArrayElements(VMContext* ctx, Arr array, Data* elements, Int mode) {
        Object* obj = reinterpret_cast<Object*>(array);
        Long length = obj->getArrayLength();
        Data* realElements = obj->getArrayElements<Data>();

        switch (mode) {
            default:
                std::cout << "warning: unknown mode was passed to ReleaseArrayElements in native interface. Defaulted to 0\n";
            case 0:
                std::memcpy(realElements, elements, length * sizeof(Data));
                std::free(elements);
                break;

            case VM_COMMIT:
                std::memcpy(realElements, elements, length * sizeof(Data));
                break;

            case VM_ABORT:
                std::free(elements);
                break;
        }
    }

    Bool* GetBoolArrayElements(VMContext* ctx, BoolArray array) {
        return GetTArrayElements<Bool, BoolArray>(ctx, array);
    }

    Byte* GetByteArrayElements(VMContext* ctx, ByteArray array) {
        return GetTArrayElements<Byte, ByteArray>(ctx, array);
    }

    Short* GetShortArrayElements(VMContext* ctx, ShortArray array) {
        return GetTArrayElements<Short, ShortArray>(ctx, array);
    }

    Int* GetIntArrayElements(VMContext* ctx, IntArray array) {
        return GetTArrayElements<Int, IntArray>(ctx, array);
    }

    Long* GetLongArrayElements(VMContext* ctx, LongArray array) {
        return GetTArrayElements<Long, LongArray>(ctx, array);
    }

    Char* GetCharArrayElements(VMContext* ctx, CharArray array) {
        return GetTArrayElements<Char, CharArray>(ctx, array);
    }

    Float* GetFloatArrayElements(VMContext* ctx, FloatArray array) {
        return GetTArrayElements<Float, FloatArray>(ctx, array);
    }

    Double* GetDoubleArrayElements(VMContext* ctx, DoubleArray array) {
        return GetTArrayElements<Double, DoubleArray>(ctx, array);
    }

    Handle* GetHandleArrayElements(VMContext* ctx, HandleArray array) {
        return GetTArrayElements<Handle, HandleArray>(ctx, array);
    }

    void ReleaseBoolArrayElements(VMContext* ctx, BoolArray array, Bool* elements, Int mode) {
        ReleaseTArrayElements<Bool, BoolArray>(ctx, array, elements, mode);
    }

    void ReleaseByteArrayElements(VMContext* ctx, ByteArray array, Byte* elements, Int mode) {
        ReleaseTArrayElements<Byte, ByteArray>(ctx, array, elements, mode);
    }

    void ReleaseShortArrayElements(VMContext* ctx, ShortArray array, Short* elements, Int mode) {
        ReleaseTArrayElements<Short, ShortArray>(ctx, array, elements, mode);
    }

    void ReleaseIntArrayElements(VMContext* ctx, IntArray array, Int* elements, Int mode) {
        ReleaseTArrayElements<Int, IntArray>(ctx, array, elements, mode);
    }

    void ReleaseLongArrayElements(VMContext* ctx, LongArray array, Long* elements, Int mode) {
        ReleaseTArrayElements<Long, LongArray>(ctx, array, elements, mode);
    }

    void ReleaseCharArrayElements(VMContext* ctx, CharArray array, Char* elements, Int mode) {
        ReleaseTArrayElements<Char, CharArray>(ctx, array, elements, mode);
    }

    void ReleaseFloatArrayElements(VMContext* ctx, FloatArray array, Float* elements, Int mode) {
        ReleaseTArrayElements<Float, FloatArray>(ctx, array, elements, mode);
    }

    void ReleaseDoubleArrayElements(VMContext* ctx, DoubleArray array, Double* elements, Int mode) {
        ReleaseTArrayElements<Double, DoubleArray>(ctx, array, elements, mode);
    }

    void ReleaseHandleArrayElements(VMContext* ctx, HandleArray array, Handle* elements, Int mode) {
        ReleaseTArrayElements<Handle, HandleArray>(ctx, array, elements, mode);
    }
}
