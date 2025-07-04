#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_LINKER_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_LINKER_H

#include "JesusVM/Module.h"

#include "JesusVM/heap/Object.h"

namespace JesusVM::Linker {
    void Init();
    void Destroy();

    void AddPath(std::string_view path);
    void AddPluginPath(std::string_view path);

    void LoadPlugin(std::string_view name);
    void LinkNativeFunction(Function* function);

    Module* AddExistingModule(Module* module);
    void LinkModule(Module* module); // runs linker stuff on a module without taking it hostage

    Module* LoadModule(Object* linker, std::string_view name);
    void RemoveModule(Object* linker, std::string_view name);

    Class* LoadClass(Module* module, std::string_view name);
    Class* LoadClass(std::string_view qualifiedName, Object* linker);
    Class* LoadPrimitive(std::string_view name);
}

#endif //JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_LINKER_H
