#include "JesusVM/Module.h"

#include "JesusVM/constpool/ConstantAscii.h"
#include "JesusVM/constpool/ConstantClass.h"

#include "JesusVM/heap/Class.h"

namespace JesusVM {
    Class::Class(Module* module)
        : mModule(module) { }

    bool Class::link(moduleweb_class_info* info) {
        mInfo = info;
        mKind = ClassKind::REGULAR;
        mModifiers = info->modifiers;

        if (linkCommon()) {
            return true;
        }

        mName = mModule->getConstPool().get<ConstantAscii>(info->name_index)->getValue();

        if (info->super_class != 0) {
            mSuperClass = mModule->getConstPool().get<ConstantClass>(info->super_class)->getClass();
        } else {
            mSuperClass = nullptr;
        }

        mFields.reserve(info->field_count);

        for (u16 i = 0; i < info->field_count; i++) {
            mFields.emplace_back(this, i);
        }

        if (mSuperClass != nullptr) {
            mMemorySize = mSuperClass->mMemorySize;
        } else {
            mMemorySize = 0;
        }


    }

    bool Class::linkPrimitive(std::string_view name) {

    }

    bool Class::linkArray(Class* base, std::string_view name) {

    }

    moduleweb_class_info* Class::getInfo() const {
        return mInfo;
    }

	Module* Class::getModule() const {
		return mModule;
	}

	std::string_view Class::getName() const {
		return mName;
	}

    bool Class::linkCommon() {
        return false; // TODO: reflection support here in future
    }
}