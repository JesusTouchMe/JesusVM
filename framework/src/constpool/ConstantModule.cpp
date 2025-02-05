#include "JesusVM/JesusVM.h"

#include "JesusVM/constpool/ConstantAscii.h"
#include "JesusVM/constpool/ConstantModule.h"

namespace JesusVM {
    ConstantModule::ConstantModule(JesusVM& vm, ConstPool& container, moduleweb_constant_module_ref_info* info)
        : Constant(type)
        , mModule(vm.getModule(container.get<ConstantAscii>(info->name_index)->getValue())) {}

    Module* ConstantModule::getModule() const {
        return mModule;
    }

    void ConstantModule::addTo(Stack::Frame* frame) {
        //TODO: implement
    }
}