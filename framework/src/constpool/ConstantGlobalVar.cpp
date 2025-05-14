// Copyright 2025 JesusTouchMe

#include "JesusVM/constpool/ConstantGlobalVar.h"
#include "JesusVM/constpool/ConstantModule.h"
#include "JesusVM/constpool/ConstantName.h"

namespace JesusVM {
    ConstantGlobalVar::ConstantGlobalVar(ConstPool& container, moduleweb_constant_global_var_ref_info* info)
        : Constant(type)
        , mGlobalVar(
                container.get<ConstantModule>(info->module_index)
                        ->getModule()
                        ->getGlobalVar(container.get<ConstantName>(info->name_info_index))
                ) {}

    GlobalVar* ConstantGlobalVar::getGlobalVar() {
        return mGlobalVar;
    }

    void ConstantGlobalVar::addTo(Stack::Frame* frame) {
        frame->pushHandle(mGlobalVar); //TODO: reflection
    }
}