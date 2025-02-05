#include "JesusVM/JesusVM.h"

#include "JesusVM/constpool/ConstantAscii.h"
#include "JesusVM/constpool/ConstantName.h"
#include "JesusVM/constpool/ConstPool.h"

namespace JesusVM {
    ConstantName::ConstantName(JesusVM& vm, ConstPool& container, moduleweb_constant_name_info* info)
        : Constant(type)
        , mName(container.get<ConstantAscii>(info->name_index)->getValue())
        , mDescriptor(container.get<ConstantAscii>(info->descriptor_index)->getValue()) { }

    std::string_view ConstantName::getName() const {
        return mName;
    }

    std::string_view ConstantName::getDescriptor() const {
        return mDescriptor;
    }

    void ConstantName::addTo(Stack::Frame* frame) {
        //TODO: implement
    }
}