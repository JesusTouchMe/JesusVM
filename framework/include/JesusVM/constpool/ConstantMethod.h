// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTMETHOD_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTMETHOD_H 1

#include "JesusVM/constpool/Constant.h"

#include "JesusVM/heap/Method.h"

namespace JesusVM {
    class ConstantMethod : public Constant {
    public:
        static constexpr Type type = Type::METHOD;

        ConstantMethod(ConstPool& container, moduleweb_constant_method_ref_info* info);

        Method* getMethod();

        void addTo(Stack::Frame* frame) override;

    private:
        Method* mMethod;
    };
}

#endif //JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTMETHOD_H