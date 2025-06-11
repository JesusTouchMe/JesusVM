// Copyright 2025 JesusTouchMe

#include "JesusVM/constpool/ConstantClass.h"
#include "JesusVM/constpool/ConstantMethod.h"
#include "JesusVM/constpool/ConstantName.h"
#include "JesusVM/constpool/ConstPool.h"

namespace JesusVM {
    ConstantMethod::ConstantMethod(ConstPool& container, moduleweb_constant_method_ref_info* info)
        : Constant(type)
        , mMethod(
            container.get<ConstantClass>(info->class_index)
                    ->getClass()
                    ->getMethod(container.get<ConstantName>(info->name_info_index))
            ) {}

    Method* ConstantMethod::getMethod() {
        return mMethod;
    }

    void ConstantMethod::addTo(Stack::Frame* frame) {
        frame->pushHandle(mMethod);
    }
}
