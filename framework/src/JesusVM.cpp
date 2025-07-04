#include "JesusVM/JesusVM.h"

#include "JesusVM/bytecode/Executor.h"

#include "JesusVM/native_interface/ProvidedFunctions.h"

#include "JesusVM/runtime/std/Primitives.h"

#include <algorithm>

namespace JesusVM {
    _JesusVMNativeInterface nativeInterface;

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
}