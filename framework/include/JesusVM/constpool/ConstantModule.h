#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTMODULE_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTMODULE_H

#include "JesusVM/Module.h"

#include "JesusVM/constpool/Constant.h"

namespace JesusVM {
    class ConstantModule : public Constant {
    public:
        static constexpr Type type = Type::MODULE;

        ConstantModule(JesusVM& vm, ConstPool& container, moduleweb_constant_module_ref_info* info);

        Module* getModule() const;

        void addTo(Stack::Frame* frame) override;

    private:
        Module* mModule;
    };
}

#endif //JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTMODULE_H
