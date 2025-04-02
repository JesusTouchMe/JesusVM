#include "JesusVM/JesusVM.h"

#include "JesusVM/constpool/ConstantFunc.h"
#include "JesusVM/constpool/ConstantModule.h"
#include "JesusVM/constpool/ConstantName.h"

namespace JesusVM {
    ConstantFunc::ConstantFunc(ConstPool& container, moduleweb_constant_function_ref_info* info)
        : Constant(type)
        , mFunc(
                container.get<ConstantModule>(info->module_index)
                        ->getModule()
                        ->getFunction(container.get<ConstantName>(info->name_info_index))
                ) {}

    Function* ConstantFunc::getFunction() {
        return mFunc;
    }

    void ConstantFunc::addTo(Stack::Frame* frame) {
        frame->pushHandle(mFunc); // TODO: when reflection gets added, push the reflection object for the function (maybe)
    }
}