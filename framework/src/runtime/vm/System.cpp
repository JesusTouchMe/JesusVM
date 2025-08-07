// Copyright 2025 JesusTouchMe

#include "JesusVM/JesusVM.h"

#include "JesusVM/runtime/vm/System.h"

namespace JesusVM::rt::vm::System {
    Module* self;

    Class* voidClass;
    Class* boolClass;
    Class* byteClass;
    Class* shortClass;
    Class* intClass;
    Class* longClass;
    Class* charClass;
    Class* floatClass;
    Class* doubleClass;
    Class* handleClass;

    Class* boolArray;
    Class* byteArray;
    Class* shortArray;
    Class* intArray;
    Class* longArray;
    Class* charArray;
    Class* floatArray;
    Class* doubleArray;
    Class* handleArray;

    namespace TrapInfo {
        Class* self;
        Field* message;
        Field* file;
        Field* line;
        Field* column;
    }

    Function* exit;
    Function* trap;
    Function* loadPlugin;
    Function* getOsName;

    [[noreturn]]
    void exit_impl(VMContext* ctx, Int code) {
        std::exit(code);
    }

    void trap_impl(VMContext* ctx, JObject infoObj) {
        Object* info = reinterpret_cast<Object*>(infoObj);

        Object* message = info->getObjectWeak(TrapInfo::message);
        std::string_view file = GetStringData(info->getObjectWeak(TrapInfo::file));
        Int line = info->getInt(TrapInfo::line);
        Int column = info->getInt(TrapInfo::column);

        Trap(message, std::string(file), line, column);
    }

    constexpr std::string_view GetOsName() {
#ifdef _WIN64
        return "Windows 64-bit";
#elif _WIN32
        return "Windows 32-bit";
#elif __APPLE__ || __MACH__
        return "MacOS";
#elif __linux__
        return "Linux";
#elif __FreeBSD__
        return "FreeBSD";
#elif __unix || __unix__
        return "Unix";
#else
        return "Other";
#endif
    }

    void loadPlugin_impl(VMContext* ctx, JObject path) {
        auto object = reinterpret_cast<Object*>(path);

        std::string_view realPath = GetStringData(object);

        Linker::LoadPlugin(realPath);
    }

    JObject getOsName_impl(VMContext* ctx) {
        std::string_view name = GetOsName();
        return ctx->NewString(name.data(), static_cast<Long>(name.size()));
    }

    namespace Object {
        Class* self;
    }
}
