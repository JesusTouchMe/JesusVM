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
            ObjectRef array = AllocPrimitiveArray(T_CHAR, static_cast<Int>(mValue.length())); // we can be certain that it's not 2.1 billion bytes long because the longest piece of literature written by humans is around 20 million characters long
            auto data = array->getArrayElements<Char>();

            std::memcpy(data, mValue.data(), static_cast<Int>(mValue.length()));

            mStringObject = AllocObject(rt::std::Primitives::String::self);
            rt::std::Primitives::String::init->invoke<void>(mStringObject.get(), array.get());
        }

        frame->pushObject(mStringObject);
    }
}