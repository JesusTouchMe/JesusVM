// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTGLOBALVAR_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTGLOBALVAR_H

#include "JesusVM/GlobalVar.h"

#include "JesusVM/constpool/Constant.h"

namespace JesusVM {
    class ConstantGlobalVar : public Constant {
    public:
        static constexpr Type type = Type::GLOBAL_VAR;

        ConstantGlobalVar(ConstPool& container, moduleweb_constant_global_var_ref_info* info);

        GlobalVar* getGlobalVar();

        void addTo(Stack::Frame* frame) override;

    private:
        GlobalVar* mGlobalVar;
    };
}

#endif //JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTGLOBALVAR_H
