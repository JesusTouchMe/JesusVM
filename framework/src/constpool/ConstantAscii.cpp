#include "JesusVM/constpool/ConstantAscii.h"

namespace JesusVM {
    ConstantAscii::ConstantAscii(JesusVM&, ConstPool&, moduleweb_constant_ascii_info* info)
    : Constant(type)
    , mValue(reinterpret_cast<const char*>(info->bytes), info->length) { }

    std::string_view ConstantAscii::getValue() const {
        return mValue;
    }

    void ConstantAscii::addTo(Stack::Frame* frame) {
        //TODO: implement
    }
}