#include "JesusVM/constpool/ConstantAscii.h"

#include "JesusVM/heap/Object.h"

#include "JesusVM/runtime/std/Primitives.h"

#include <cstring>

namespace JesusVM {
    ConstantAscii::ConstantAscii(ConstPool&, moduleweb_constant_ascii_info* info)
        : Constant(type)
        , mValue(reinterpret_cast<const char*>(info->bytes), info->length) { }

    std::string_view ConstantAscii::getValue() const {
        return mValue;
    }

    void ConstantAscii::addTo(Stack::Frame* frame) {
        if (mStringObject == nullptr) {
            ObjectRef array = AllocPrimitiveArray(T_CHAR, static_cast<Long>(mValue.length()));
            auto data = array->getArrayElements<Char>();

            std::memcpy(data, mValue.data(), static_cast<Long>(mValue.length()));

            mStringObject = AllocObject(rt::std::Primitives::String::self);
            mStringObject->setObject(rt::std::Primitives::String::data, array);
        }

        frame->pushObject(mStringObject);
    }
}