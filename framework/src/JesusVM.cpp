#include "JesusVM/JesusVM.h"

#include "JesusVM/bytecode/Executor.h"

#include "JesusVM/native_interface/ProvidedFunctions.h"

#include "JesusVM/runtime/std/Primitives.h"

#include <cstring>

namespace JesusVM {
    _JesusVMNativeInterface nativeInterface;

    std::vector<void (*)(VMContext*, TrapInfo*)> trapHooks;

    void Init() {
        Executor::InitDispatch();

        NativeInterface::Prepare(&nativeInterface);
    }

    VMContext* GetContext() {
        return reinterpret_cast<VMContext*>(&nativeInterface);
    }

    ObjectRef CreateString(std::string_view data) {
        ObjectRef array = AllocPrimitiveArray(T_CHAR, static_cast<Long>(data.length()));
        auto elements = array->getArrayElements<Char>();

        std::memcpy(elements, data.data(), static_cast<Long>(data.length()));

        auto object = AllocObject(rt::std::Primitives::String::self);
        object->setObject(rt::std::Primitives::String::data, array);

        return std::move(object);
    }

    std::string_view GetStringData(Object* object) {
        if (!object->getClass()->isAssignableTo(rt::std::Primitives::String::self)) {
            std::cout << "insert type mismatch error\n";
            std::exit(1);
        }

        Object* data = object->getObjectWeak(rt::std::Primitives::String::data);
        Char* cString = data->getArrayElements<Char>();
        Int length = data->getArrayLength();

        return { cString, static_cast<std::size_t>(length) };
    }

    void Trap(Object* message, std::string file, Int line, Int column) {
        TrapInfo trapInfo;
        trapInfo.message = reinterpret_cast<JObject>(message);
        trapInfo.location.file = file.c_str();
        trapInfo.location.line = line;
        trapInfo.location.column = column;
        trapInfo.recoverable = false;

        for (auto hook : trapHooks) {
            hook(GetContext(), &trapInfo);
        }

        //TODO: this should be recoverable, but we still unwind the stack and start from a handler or something
        if (message->getClass()->isAssignableTo(rt::std::Primitives::String::self)) {
            std::cout << GetStringData(message) << "\n";
        }
        std::exit(1);
    }

    void AddTrapHook(void(*hook)(VMContext*, TrapInfo*)) {
        if (hook != nullptr) trapHooks.push_back(hook);
    }
}
