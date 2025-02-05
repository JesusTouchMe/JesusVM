#ifndef JESUSVM_CONSTANTFUNC_H
#define JESUSVM_CONSTANTFUNC_H

#include "JesusVM/Function.h"

#include "JesusVM/constpool/Constant.h"

namespace JesusVM {
    class ConstantFunc : public Constant {
    public:
        static constexpr Type type = Type::FUNCTION;

        ConstantFunc(JesusVM& vm, ConstPool& container, moduleweb_constant_function_ref_info* info);

        Function* getFunction();

        void addTo(Stack::Frame* frame) override;

    private:
        Function* mFunc;
    };
}

#endif //JESUSVM_CONSTANTFUNC_H
