// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTCLASS_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTCLASS_H 1

#include "JesusVM/constpool/Constant.h"

#include "JesusVM/heap/Class.h"

namespace JesusVM {
    class ConstantClass : public Constant {
    public:
        static constexpr Type type = Type::CLASS;

        ConstantClass(JesusVM& vm, ConstPool& container, moduleweb_constant_class_ref_info* info);

        Class* getClass() const;

        void addTo(Stack::Frame* frame) override;

    private:
        Class* mClass;
    };
}

#endif // JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTCLASS_H
