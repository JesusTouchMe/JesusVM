// Copyright 2025 JesusTouchMe

#include "JesusVM/JesusVM.h"

#include "JesusVM/constpool/Constant.h"
#include "JesusVM/constpool/ConstantAscii.h"
#include "JesusVM/constpool/ConstantClass.h"
#include "JesusVM/constpool/ConstantField.h"
#include "JesusVM/constpool/ConstantFunc.h"
#include "JesusVM/constpool/ConstantGlobalVar.h"
#include "JesusVM/constpool/ConstantMethod.h"
#include "JesusVM/constpool/ConstantModule.h"
#include "JesusVM/constpool/ConstantName.h"
#include "JesusVM/constpool/ConstPool.h"

namespace JesusVM {
    std::unique_ptr<Constant> Constant::Create(ConstPool& container, moduleweb_constant_info* info) {
        auto type = static_cast<moduleweb_constant_type_e>(info->type); // do this so compiler complains at new ones. should be optimized away anyways

        switch (type) {
            case MODULEWEB_CONSTANT_TYPE_ASCII:
                return std::make_unique<ConstantAscii>(container, &info->ascii_info);
            case MODULEWEB_CONSTANT_TYPE_NAME:
                return std::make_unique<ConstantName>(container, &info->name_info);
            case MODULEWEB_CONSTANT_TYPE_MODULE_REF:
                return std::make_unique<ConstantModule>(container, &info->module_ref_info);
            case MODULEWEB_CONSTANT_TYPE_GLOBAL_VAR_REF:
                return std::make_unique<ConstantGlobalVar>(container, &info->global_var_ref_info);
            case MODULEWEB_CONSTANT_TYPE_FUNCTION_REF:
                return std::make_unique<ConstantFunc>(container, &info->function_ref_info);
            case MODULEWEB_CONSTANT_TYPE_CLASS_REF:
                return std::make_unique<ConstantClass>(container, &info->class_ref_info);
            case MODULEWEB_CONSTANT_TYPE_FIELD_REF:
                return std::make_unique<ConstantField>(container, &info->field_ref_info);
            case MODULEWEB_CONSTANT_TYPE_METHOD_REF:
                return std::make_unique<ConstantMethod>(container, &info->method_ref_info);
            default:
                return nullptr;
        }
    }
}