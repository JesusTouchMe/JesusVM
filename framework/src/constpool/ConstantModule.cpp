#include "JesusVM/JesusVM.h"

#include "JesusVM/constpool/ConstantAscii.h"
#include "JesusVM/constpool/ConstantModule.h"

namespace JesusVM {
    ConstantModule::ConstantModule(ConstPool& container, moduleweb_constant_module_ref_info* info)
        : Constant(type)
        , mModule(Linker::LoadModule(nullptr, container.get<ConstantAscii>(info->name_index)->getValue())) {}

    Module* ConstantModule::getModule() const {
        return mModule;
    }

    void ConstantModule::addTo(Stack::Frame* frame) {
        frame->pushHandle(mModule); // TODO: when reflection gets added, push the reflection object for the module
    }
}