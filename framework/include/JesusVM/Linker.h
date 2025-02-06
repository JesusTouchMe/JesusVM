#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_LINKER_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_LINKER_H

#include "JesusVM/Module.h"

#include "JesusVM/heap/Object.h"

namespace JesusVM::Linker {
    void Init(JesusVM& vm);
    void Destroy();

    void AddPath(std::string_view path);

    Module* LoadModule(Object* linker, std::string_view name);

    Class* LoadClass(Object* linker, Module* module, std::string_view name);
    Class* LoadClass(Object* linker, std::string_view qualifiedName);
    Class* LoadPrimitive(std::string_view name);
}

#endif //JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_LINKER_H
