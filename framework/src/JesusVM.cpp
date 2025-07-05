#include "JesusVM/JesusVM.h"

#include "JesusVM/bytecode/Executor.h"

#include "JesusVM/native_interface/ProvidedFunctions.h"

#include "JesusVM/runtime/std/Primitives.h"

#include <algorithm>

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

        if (!trapInfo.recoverable) {
            //TODO: print error info, but it's an object so can't just use GetStringData
            std::exit(1);
        }
    }

    void AddTrapHook(void(*hook)(VMContext*, TrapInfo*)) {
        if (hook != nullptr) trapHooks.push_back(hook);
    }
}
