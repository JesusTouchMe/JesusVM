// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTFIELD_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTFIELD_H 1

#include "JesusVM/constpool/Constant.h"

#include "JesusVM/heap/Field.h"

namespace JesusVM {
    class ConstantField : public Constant {
    public:
        static constexpr Type type = Type::FIELD;

        ConstantField(ConstPool& container, moduleweb_constant_field_ref_info* info);

        Field* getField();

        void addTo(Stack::Frame* frame) override;

    private:
        Field* mField;
    };
}

#endif // JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTFIELD_H
