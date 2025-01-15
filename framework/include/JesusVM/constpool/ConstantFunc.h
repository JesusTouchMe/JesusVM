#ifndef JESUSVM_CONSTANTFUNC_H
#define JESUSVM_CONSTANTFUNC_H

#include "types.h"

#include "JesusVM/Function.h"

#include "JesusVM/constpool/Constant.h"

namespace JesusVM {
    class ConstantFunc : public Constant {
    public:
        explicit ConstantFunc(Function* func);

        Function* getFunction();

        void addTo(Stack::Frame* frame) override;

    private:
        Function* mFunc;
    };
}

#endif //JESUSVM_CONSTANTFUNC_H
