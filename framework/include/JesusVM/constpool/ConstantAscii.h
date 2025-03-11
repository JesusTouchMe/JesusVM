#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTASCII_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTASCII_H

#include "JesusVM/constpool/Constant.h"

#include "JesusVM/heap/Object.h"

namespace JesusVM {
    class ConstantAscii : public Constant {
    public:
        static constexpr Type type = Type::ASCII;

        ConstantAscii(JesusVM&, ConstPool&, moduleweb_constant_ascii_info* info);

        std::string_view getValue() const;

        void addTo(Stack::Frame* frame) override;

    private:
        std::string_view mValue;
        ObjectRef mStringObject = nullptr;
    };
}

#endif //JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTASCII_H
