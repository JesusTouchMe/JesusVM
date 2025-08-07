// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_RUNTIME_VM_SYSTEM_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_RUNTIME_VM_SYSTEM_H 1

#include "JesusVM/Module.h"

namespace JesusVM::rt::vm::System {
    extern Module* self;

    extern Class* voidClass;
    extern Class* boolClass;
    extern Class* byteClass;
    extern Class* shortClass;
    extern Class* intClass;
    extern Class* longClass;
    extern Class* charClass;
    extern Class* floatClass;
    extern Class* doubleClass;
    extern Class* handleClass;

    extern Class* boolArray;
    extern Class* byteArray;
    extern Class* shortArray;
    extern Class* intArray;
    extern Class* longArray;
    extern Class* charArray;
    extern Class* floatArray;
    extern Class* doubleArray;
    extern Class* handleArray;

    namespace TrapInfo {
        extern Class* self;
        extern Field* message;
        extern Field* file;
        extern Field* line;
        extern Field* column;
    }

    extern Function* exit;
    extern Function* trap;
    extern Function* loadPlugin;
    extern Function* getOsName;

    [[noreturn]]
    void exit_impl(VMContext* ctx, Int code);

    void trap_impl(VMContext* ctx, JObject message);

    void loadPlugin_impl(VMContext* ctx, JObject pluginName);

    JObject getOsName_impl(VMContext* ctx);
}

#endif // JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_RUNTIME_VM_SYSTEM_H
