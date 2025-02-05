// Copyright 2025 JesusTouchMe

#include "JesusVM/JesusVM.h"

#include "JesusVM/constpool/ConstantAscii.h"
#include "JesusVM/constpool/ConstantClass.h"
#include "JesusVM/constpool/ConstantModule.h"

namespace JesusVM {
    ConstantClass::ConstantClass(JesusVM& vm, ConstPool& container, moduleweb_constant_class_ref_info* info)
        : Constant(type)
        , mClass(container.get<ConstantModule>(info->module_index)->getModule()
                        ->getClass(container.get<ConstantAscii>(info->name_index)->getValue())) { }


    Class* ConstantClass::getClass() const {
        return mClass;
    }

    void ConstantClass::addTo(Stack::Frame* frame) {
        //TODO: implement
    }
}