// Copyright 2025 JesusTouchMe

#include "JesusVM/GlobalVar.h"
#include "JesusVM/Module.h"

namespace JesusVM {
    GlobalVar::GlobalVar(Module* module, u16 index)
        : mModule(module)
        , mIndex(index) {
        moduleweb_global_var_info* info = &module->getInfo()->global_vars[index];

        auto name = module->getConstPool().get<ConstantName>(info->name_index);
        mName = name->getName();
        mDescriptor = name->getDescriptor();

        u32 index2 = 0;

        if (mType.parse(mDescriptor, &index2)) {
            std::cout << "Field descriptor issue\n";
            std::exit(1);
        }
    }

    Module* GlobalVar::getModule() const {
        return mModule;
    }

    moduleweb_global_var_info* GlobalVar::getInfo() const {
        return &mModule->getInfo()->global_vars[mIndex];
    }

    const TypeInfo& GlobalVar::getType() const {
        return mType;
    }

    std::string_view GlobalVar::getName() const {
        return mName;
    }

    std::string_view GlobalVar::getDescriptor() const {
        return mDescriptor;
    }

    GlobalVar::Modifiers GlobalVar::getModifiers() const {
        return getInfo()->modifiers;
    }

    u16 GlobalVar::getIndex() const {
        return mIndex;
    }

    JValue GlobalVar::getValue() const {
        return mValue;
    }

    void GlobalVar::setValue(JValue value) {
        mValue = value;
    }
}