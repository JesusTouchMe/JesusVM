// Copyright 2025 JesusTouchMe

#include "JesusVM/JesusVM.h"

#include "JesusVM/constpool/Constant.h"
#include "JesusVM/constpool/ConstantAscii.h"
#include "JesusVM/constpool/ConstantClass.h"
#include "JesusVM/constpool/ConstantField.h"
#include "JesusVM/constpool/ConstantFunc.h"
#include "JesusVM/constpool/ConstantModule.h"
#include "JesusVM/constpool/ConstantName.h"
#include "JesusVM/constpool/ConstPool.h"

namespace JesusVM {
    std::unique_ptr<Constant> Constant::Create(JesusVM& vm, ConstPool& container, moduleweb_constant_info* info) {
        auto type = static_cast<moduleweb_constant_type_e>(info->type); // do this so compiler complains at new ones. should be optimized away anyways

        switch (type) {
            case MODULEWEB_CONSTANT_TYPE_ASCII:
                return std::make_unique<ConstantAscii>(vm, container, &info->ascii_info);
            case MODULEWEB_CONSTANT_TYPE_NAME:
                return std::make_unique<ConstantName>(vm, container, &info->name_info);
            case MODULEWEB_CONSTANT_TYPE_MODULE_REF:
                return std::make_unique<ConstantModule>(vm, container, &info->module_ref_info);
            case MODULEWEB_CONSTANT_TYPE_FUNCTION_REF:
                return std::make_unique<ConstantFunc>(vm, container, &info->function_ref_info);
            case MODULEWEB_CONSTANT_TYPE_CLASS_REF:
                return std::make_unique<ConstantClass>(vm, container, &info->class_ref_info);
            case MODULEWEB_CONSTANT_TYPE_FIELD_REF:
                return std::make_unique<ConstantField>(vm, container, &info->field_ref_info);
            case MODULEWEB_CONSTANT_TYPE_AMOUNT:
                return nullptr;
        }
    }
}