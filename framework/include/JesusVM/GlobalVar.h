// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_GLOBALVAR_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_GLOBALVAR_H 1

#include "types.h"

#include "JesusVM/JesusNative.h"

#include "JesusVM/type/Type.h"

#include "moduleweb/global_var_info.h"

namespace JesusVM {
    class Module;

    class GlobalVar {
    public:
        using Modifiers = u16;

        GlobalVar(Module* module, u16 index);

        Module* getModule() const;
        moduleweb_global_var_info* getInfo() const;
        const TypeInfo& getType() const;
        std::string_view getName() const;
        std::string_view getDescriptor() const;
        Modifiers getModifiers() const;
        u16 getIndex() const;

        JValue getValue() const;
        void setValue(JValue value);

    private:
        Module* mModule;
        u16 mIndex;

        TypeInfo mType;
        std::string_view mName;
        std::string_view mDescriptor;

        JValue mValue;
    };
}

#endif // JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_GLOBALVAR_H
