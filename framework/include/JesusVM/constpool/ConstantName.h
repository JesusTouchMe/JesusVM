#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTNAME_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTNAME_H

#include "JesusVM/constpool/Constant.h"

namespace JesusVM {
    class ConstantName : public Constant {
    public:
        static constexpr Type type = Type::NAME;

        ConstantName(JesusVM& vm, ConstPool& container, moduleweb_constant_name_info* info);

        std::string_view getName() const;
        std::string_view getDescriptor() const;

        void addTo(Stack::Frame* frame) override;

    private:
        std::string_view mName;
        std::string_view mDescriptor;
    };
}

#endif //JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_CONSTPOOL_CONSTANTNAME_H
