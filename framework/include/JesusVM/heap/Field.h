#ifndef JESUSVM_FIELD_H
#define JESUSVM_FIELD_H

#include "JesusVM/type/Type.h"

#include "moduleweb/field_info.h"

#include <string_view>

namespace JesusVM {
    class Class;

    class Field {
    friend class Class;
    friend bool ParseFieldType(Field*);
    public:
        Field(Class* clas, u32 index);

        Class* getClass() const;
        moduleweb_field_info* getInfo() const;
        const TypeInfo& getType() const;
        std::string_view getName() const;
        std::string_view getDescriptor() const;
        u32 getIndex() const;
        u32 getMemoryOffset() const;

    private:
        Class* mClass;
        u32 mIndex;
        u32 mMemoryOffset;

        TypeInfo mType;
        std::string_view mName;
        std::string_view mDescriptor;
    };
}

#endif //JESUSVM_FIELD_H
