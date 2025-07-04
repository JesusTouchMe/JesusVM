// Copyright 2025 JesusTouchMe

#include "JesusVM/Linker.h"
#include "JesusVM/Preload.h"

#include "JesusVM/runtime/std/Primitives.h"
#include "JesusVM/runtime/vm/System.h"

#include <any>
#include <array>

#undef OPTIONAL

namespace JesusVM::Preload {
    bool finished = false;

    struct ModuleEntry {
        std::string_view name;
        Module** module;
        bool optional;
    };

    struct ClassEntry {
        Module** module; // owner
        std::string_view name;
        Class** clas;
        bool optional;
    };

    struct FieldEntry {
        Class** owner;
        std::string_view name;
        std::string_view desc;
        Field** field;
        bool optional;
    };

    struct FunctionEntry {
        Module** module; // owner
        std::string_view name;
        std::string_view desc;
        Function** function;
        bool optional;
    };

    struct NativeOverrideEntry {
        Function** function;
        void* entry;
    };

#   define PRELOAD_MODULE(name, var, opt) { name, &var, opt },
    ModuleEntry entries[] = {
#       include "JesusVM/preload/Preload-Modules.h"
    };
#   undef PRELOAD_MODULE

#   define PRELOAD_CLASS(module, name, var, opt) { &module, name, &var, opt },
    ClassEntry classEntries[] = {
#       include "JesusVM/preload/Preload-Classes.h"
    };
#   undef PRELOAD_CLASS

    ClassEntry primitiveEntries[] = {
            { &rt::vm::System::self, "void", &rt::vm::System::voidClass, false },
            { &rt::vm::System::self, "bool", &rt::vm::System::boolClass, false },
            { &rt::vm::System::self, "byte", &rt::vm::System::byteClass, false },
            { &rt::vm::System::self, "short", &rt::vm::System::shortClass, false },
            { &rt::vm::System::self, "int", &rt::vm::System::intClass, false },
            { &rt::vm::System::self, "long", &rt::vm::System::longClass, false },
            { &rt::vm::System::self, "char", &rt::vm::System::charClass, false },
            { &rt::vm::System::self, "float", &rt::vm::System::floatClass, false },
            { &rt::vm::System::self, "double", &rt::vm::System::doubleClass, false },
            { &rt::vm::System::self, "handle", &rt::vm::System::handleClass, false },
    };

    ClassEntry primitiveArrayEntries[] = {
            { &rt::vm::System::self, "[Z", &rt::vm::System::boolArray, false },
            { &rt::vm::System::self, "[B", &rt::vm::System::byteArray, false },
            { &rt::vm::System::self, "[S", &rt::vm::System::shortArray, false },
            { &rt::vm::System::self, "[I", &rt::vm::System::intArray, false },
            { &rt::vm::System::self, "[L", &rt::vm::System::longArray, false },
            { &rt::vm::System::self, "[C", &rt::vm::System::charArray, false },
            { &rt::vm::System::self, "[F", &rt::vm::System::floatArray, false },
            { &rt::vm::System::self, "[D", &rt::vm::System::doubleArray, false },
            { &rt::vm::System::self, "[H", &rt::vm::System::handleArray, false },
    };

#   define PRELOAD_FIELD(owner, name, desc, var, opt) { &owner, name, desc, &var, opt },
    FieldEntry fieldEntries[] = {
#       include "JesusVM/preload/Preload-Fields.h"
    };
#   undef PRELOAD_FIELD

#   define PRELOAD_FUNCTION(module, name, desc, var, opt) { &module, name, desc, &var, opt },
    FunctionEntry functionEntries[] = {
#       include "JesusVM/preload/Preload-Functions.h"
    };
#   undef PRELOAD_FUNCTION

    NativeOverrideEntry nativeOverrideEntries[] = {
            { &rt::vm::System::exit, reinterpret_cast<void*>(rt::vm::System::exit_impl) },
            { &rt::vm::System::trap, reinterpret_cast<void*>(rt::vm::System::trap_impl) },
            { &rt::vm::System::loadPlugin, reinterpret_cast<void*>(rt::vm::System::loadPlugin_impl) },
    };

    void PreloadSystemModules() {
        finished = false;

        for (auto& entry : entries) {
            Module* module = Linker::LoadModule(nullptr, entry.name);
            if (module == nullptr) {
                if (entry.optional) continue;

                std::cout << "warning: preload for module " << entry.name << " failed\n";
                std::exit(1);
            }

            *entry.module = module;
        }

        for (auto& entry : classEntries) {
            Class* clas = Linker::LoadClass(*entry.module, entry.name);
            if (clas == nullptr) {
                if (entry.optional) continue;

                std::cout << "warning: preload for class " << entry.name << " failed\n";
                std::exit(1);
            }

            *entry.clas = clas;
        }

        for (auto& entry : primitiveEntries) {
            Class* clas = Linker::LoadPrimitive(entry.name);
            if (clas == nullptr) {
                if (entry.optional) continue;

                std::cout << "warning: preload for primitive class " << entry.name << " failed\n";
                std::exit(1);
            }

            *entry.clas = clas;
        }

        for (auto& entry : primitiveArrayEntries) {
            Class* clas = Linker::LoadClass(entry.name, nullptr); // load by qualified name so entries need to ensure they use that
            if (clas == nullptr) {
                if (entry.optional) continue;

                std::cout << "warning: preload for class " << entry.name << " failed\n";
                std::exit(1);
            }

            *entry.clas = clas;
        }

        for (auto& entry : fieldEntries) {
            Field* field = (*entry.owner)->getField(entry.name, entry.desc);
            if (field == nullptr) {
                if (entry.optional) continue;

                std::cout << "warning: preload for field " << entry.name << " failed\n";
                std::exit(1);
            }

            *entry.field = field;
        }

        for (auto& entry : functionEntries) {
            Function* function = (*entry.module)->getFunction(entry.name, entry.desc);
            if (function == nullptr) {
                if (entry.optional) continue;

                std::cout << "warning: preload for function " << entry.name << " failed\n";
                std::exit(1);
            }

            *entry.function = function;
        }

        for (auto& entry : nativeOverrideEntries) {
            Function* func = *entry.function;

            func->mInfo->modifiers |= MODULEWEB_FUNCTION_MODIFIER_NATIVE;
            func->mCodeAttribute.code = static_cast<u8*>(entry.entry);
        }

        Module* initializer = Linker::LoadModule(nullptr, "vm/Initializer");
        if (initializer == nullptr) {
            std::cout << "warning: system initializer module was not found\n";
        }

        finished = true;
    }
}
