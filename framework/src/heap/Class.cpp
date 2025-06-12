#include "JesusVM/JesusVM.h"

#include "JesusVM/constpool/ConstantAscii.h"
#include "JesusVM/constpool/ConstantClass.h"

#include "JesusVM/heap/Class.h"

#include "JesusVM/runtime/std/Primitives.h"

#include <algorithm>

namespace JesusVM {
    static inline u64 align_up(u64 x, u64 alignment) {
        return (x + (alignment - 1)) & ~(alignment - 1);
    }

    Class::Class(Module* module, moduleweb_class_info* info)
        : mInfo(info)
        , mModule(module)
        , mState(ClassState::INITIALIZED)
        , mWaitingThreads(0) { }

    bool Class::link() {
        mState = ClassState::LINKING;

        linkCommon();

        mKind = ClassKind::REGULAR;
        mModifiers = mInfo->modifiers;

        mName = mModule->getConstPool().get<ConstantAscii>(mInfo->name_index)->getValue();

        if (mInfo->super_class != 0) {
            mSuperClass = mModule->getConstPool().get<ConstantClass>(mInfo->super_class)->getClass();
        } else {
            mSuperClass = nullptr;
        }

        mFields.reserve(mInfo->field_count);
        for (u16 i = 0; i < mInfo->field_count; i++) {
            mFields.emplace_back(this, i);
        }

        mMethods.reserve(mInfo->method_count);
        for (u16 i = 0; i < mInfo->method_count; i++) {
            mMethods.emplace_back(this, i);
        }

        if (mSuperClass != nullptr) {
            mMemorySize = mSuperClass->mMemorySize;
            mVTable = mSuperClass->mVTable;
        } else {
            mMemorySize = 0;
        }

        std::array<FieldBucket, static_cast<u64>(Type::TYPE_COUNT)> buckets;
        for (Field& field : mFields) {
            FieldBucket& bucket = buckets[static_cast<u64>(field.mType.getInternalType())];
            bucket.fields.push_back(&field);
        }

        orderFieldBuckets(buckets);

        for (auto& method : mMethods) {
            auto it = std::find_if(mVTable.begin(), mVTable.end(), [&method](Method* method1) {
                return method1->getName() == method.getName() && method1->getDescriptor() == method.getDescriptor();
            });

            if (it != mVTable.end()) {
                auto index = std::distance(mVTable.begin(), it);
                mVTable[index] = &method;
                method.mVTableIndex = index;
            } else {
                mVTable.push_back(&method);
                method.mVTableIndex = mVTable.size() - 1;
            }
        }

        mState = ClassState::LINKED;

        return false;
    }

    bool Class::linkPrimitive(std::string_view name) {
        mState = ClassState::LINKING;

        linkCommon();

        mInfo = nullptr;
        mKind = ClassKind::PRIMITIVE;
        mModifiers = MODULEWEB_CLASS_MODIFIER_PUBLIC | MODULEWEB_CLASS_MODIFIER_FINAL | MODULEWEB_CLASS_MODIFIER_ABSTRACT;

        mName = name;
        mSuperClass = nullptr;
        mMemorySize = 0;
        mState = ClassState::LINKED;

        return false;
    }

    bool Class::linkArray(Class* base, std::string_view name) {
        mState = ClassState::LINKING;

        linkCommon();

        mInfo = nullptr;
        mKind = ClassKind::ARRAY;
        mModifiers = base->mModifiers | MODULEWEB_CLASS_MODIFIER_FINAL | MODULEWEB_CLASS_MODIFIER_ABSTRACT;

        mName = name;
        mArrayBaseClass = base;

        mSuperClass = rt::std::Primitives::Object::self;
        mVTable = rt::std::Primitives::Object::self->mVTable;

        mMemorySize = 0;

        mState = ClassState::LINKED;

        return false;
    }

    moduleweb_class_info* Class::getInfo() const {
        return mInfo;
    }

	Module* Class::getModule() const {
		return mModule;
	}

    ClassKind Class::getKind() const {
        return mKind;
    }

    ClassState Class::getState() const {
        return mState;
    }

	std::string_view Class::getName() const {
		return mName;
	}

    u64 Class::getFieldBufferSize() const {
        return mMemorySize;
    }

    u64 Class::getTotalSize() const {
        return getFieldBufferSize() + align_up(sizeof(Object), 16);
    }

    bool Class::isAssignableTo(Class* other) const {
        if (other == nullptr) return false;

        const Class* current = this;
        while (current != nullptr) {
            if (current == other) return true;
            current = current->mSuperClass;
        }

        return false;
    }

    Field* Class::getField(std::string_view name, std::string_view descriptor) {
        auto it = std::find_if(mFields.begin(), mFields.end(), [name, descriptor](Field& field) {
            return field.getName() == name && field.getDescriptor() == descriptor;
        });
        if (it != mFields.end()) return &*it;

        return nullptr;
    }

    Field* Class::getField(ConstantName* name) {
        return getField(name->getName(), name->getDescriptor());
    }

    Method* Class::getMethod(std::string_view name, std::string_view descriptor) {
        auto it = std::find_if(mMethods.begin(), mMethods.end(), [name, descriptor](Method& method) {
            return method.getName() == name && method.getDescriptor() == descriptor;
        });
        if (it != mMethods.end()) return &*it;

        return nullptr;
    }

    Method* Class::getMethod(ConstantName* name) {
        return getMethod(name->getName(), name->getDescriptor());
    }

    Function* Class::dispatchMethod(Method* method) const {
        return mVTable[method->mVTableIndex]->mFunction;
    }

    bool Class::isPublic() const {
        return mModifiers & MODULEWEB_CLASS_MODIFIER_PUBLIC;
    }

    bool Class::isPrivate() const {
        return mModifiers & MODULEWEB_CLASS_MODIFIER_PRIVATE;
    }

    bool Class::isAbstract() const {
        return mModifiers & MODULEWEB_CLASS_MODIFIER_ABSTRACT;
    }

    bool Class::isFinal() const {
        return mModifiers & MODULEWEB_CLASS_MODIFIER_FINAL;
    }

    void Class::linkCommon() {
        mLoadingThread = Threading::CurrentThread::GetHandle();
    }

    void Class::orderFieldBuckets(std::array<FieldBucket, static_cast<u64>(Type::TYPE_COUNT)>& buckets) {
        u64 offset = mMemorySize;

        offset = ALIGN(offset, sizeof(void*));
        orderFieldBucket(&buckets[static_cast<u64>(Type::REFERENCE)], sizeof(void*), offset);

        offset = ALIGN(offset, 8);

#ifndef JESUSVM_CONFIG_32BIT
        orderFieldBucket(&buckets[static_cast<u64>(Type::HANDLE)], 8, offset);
#endif

        orderFieldBucket(&buckets[static_cast<u64>(Type::DOUBLE)], 8, offset);
        orderFieldBucket(&buckets[static_cast<u64>(Type::LONG)], 8, offset);

#ifdef JESUSVM_CONFIG_32BIT
        orderFieldBucket(&buckets[static_cast<u64>(Type::HANDLE)], 4, offset);
#endif

        orderFieldBucket(&buckets[static_cast<u64>(Type::FLOAT)], 4, offset);
        orderFieldBucket(&buckets[static_cast<u64>(Type::INT)], 4, offset);

        orderFieldBucket(&buckets[static_cast<u64>(Type::SHORT)], 2, offset);

        orderFieldBucket(&buckets[static_cast<u64>(Type::BYTE)], 1, offset);
        orderFieldBucket(&buckets[static_cast<u64>(Type::CHAR)], 1, offset);
        orderFieldBucket(&buckets[static_cast<u64>(Type::BOOL)], 1, offset);

        mMemorySize = offset;
    }

    void Class::orderFieldBucket(Class::FieldBucket* bucket, u32 size, u64& offset) {
        u64 tempOffset = offset;

        for (Field* field : bucket->fields) {
            field->mMemoryOffset = tempOffset;
            tempOffset += size;
        }

        offset = tempOffset;
    }
}
