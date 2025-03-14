// Copyright 2025 JesusTouchMe

#include "JesusVM/constpool/ConstantClass.h"
#include "JesusVM/constpool/ConstantField.h"
#include "JesusVM/constpool/ConstantName.h"
#include "JesusVM/constpool/ConstPool.h"

namespace JesusVM {
    ConstantField::ConstantField(ConstPool& container, moduleweb_constant_field_ref_info* info)
        : Constant(type)
        , mField(
                container.get<ConstantClass>(info->class_index)
                        ->getClass()
                        ->getField(container.get<ConstantName>(info->name_info_index))
                ) {}

    Field* ConstantField::getField() {
        return mField;
    }

    void ConstantField::addTo(Stack::Frame* frame) {
        frame->pushHandle(mField); // TODO: when reflection gets added, push the reflection object for the field
    }
}