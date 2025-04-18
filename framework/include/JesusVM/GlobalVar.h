// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_GLOBALVAR_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_GLOBALVAR_H 1

#include "types.h"

#include "JesusVM/type/Type.h"

#include "moduleweb/global_var_info.h"

namespace JesusVM {
    class Module;

    class GlobalVar {
    public:
        using Modifiers = u16;

        GlobalVar(Module* module, moduleweb_global_var_info* info);

        moduleweb_global_var_info* getInfo() const;
        Module* getModule() const;
        const TypeInfo& getType() const;
        std::string_view getName() const;
        std::string_view getDescriptor() const;
        Modifiers getModifiers() const;
    };
}

#endif // JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_GLOBALVAR_H
