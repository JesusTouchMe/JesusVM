#include "JesusVM/Module.h"

#include "JesusVM/constpool/ConstantAscii.h"

#include "JesusVM/heap/Class.h"
#include "JesusVM/heap/Field.h"

namespace JesusVM {
    Field::Field(Class* clas, u32 index)
        : mClass(clas)
        , mIndex(index)
        , mMemoryOffset() {
        moduleweb_field_info* info = &clas->getInfo()->fields[index];

        auto name = clas->getModule()->getConstPool().get<ConstantName>(info->name_index);
        mName = name->getName();
        mDescriptor = name->getDescriptor();

        if (ParseFieldType(this)) {
            std::cout << "Field descriptor issue\n";
            std::exit(1);
        }
    }

    moduleweb_field_info* Field::getInfo() const {
        return &mClass->getInfo()->fields[mIndex];
    }

    const TypeInfo& Field::getType() const {
        return mType;
    }

    std::string_view Field::getName() const {
        return mName;
    }

    std::string_view Field::getDescriptor() const {
        return mDescriptor;
    }

    u32 Field::getIndex() const {
        return mIndex;
    }

    u32 Field::getMemoryOffset() const {
        return mMemoryOffset;
    }
}