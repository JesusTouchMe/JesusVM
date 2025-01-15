#ifndef JESUSVM_FIELD_H
#define JESUSVM_FIELD_H

#include "JesusVM/type/Type.h"

#include <string_view>

namespace JesusVM {
    class Class;

    class Field {
    public:

    private:
        Class* mClass;
        u32 mIndex;
        u32 mMemoryOffset;

        Type* mType;
        std::string_view mName;
    };
}

#endif //JESUSVM_FIELD_H
