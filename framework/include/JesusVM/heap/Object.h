#ifndef JESUSVM_OBJECT_H
#define JESUSVM_OBJECT_H

#include "JesusVM/JesusNative.h"

#include "JesusVM/heap/Class.h"

#include <atomic>

namespace JesusVM {
    enum PrimitiveTypeID {
        T_BOOL,
        T_BYTE,
        T_SHORT,
        T_INT,
        T_LONG
    };

    struct Array;

    class Object {
    friend struct Array;
    friend Object* AllocObject(Class* clas);
    friend Object* AllocArray(Class* clas, Int size);
    public:
        Class* getClass();
        i32 getReferenceCount() const;

        Array* toArray();
        Int getArrayLength();

        bool isInstance(Class* clas);

        void addReference();
        void removeReference();

        u8* getFieldsBuffer();

        template <typename T>
        constexpr T* getArrayElements();

        Bool getBoolean(Field* field) const;
        Byte getByte(Field* field) const;
        Short getShort(Field* field) const;
        Int getInt(Field* field) const;
        Long getLong(Field* field) const;
        Object* getObject(Field* field) const;

        void setBoolean(Field* field, Bool value);
        void setByte(Field* field, Byte value);
        void setShort(Field* field, Short value);
        void setInt(Field* field, Int value);
        void setLong(Field* field, Long value);
        void setObject(Field* field, Object* value);

    private:
        Object(Class* clas);

        Class* mClass;

        std::atomic<i32> mRefCount;
    };

    class ObjectRef {
    public:
        ObjectRef(Object* object) : mObject(object) {
            if (mObject != nullptr)
                mObject->addReference();
        }

        ObjectRef(Object* object, bool) : mObject(object) {}

        ObjectRef(std::nullptr_t = nullptr) : mObject(nullptr) {}

        ObjectRef(const ObjectRef& other) : mObject(other.mObject) {
            if (mObject != nullptr) {
                mObject->addReference();
            }
        }

        ObjectRef(ObjectRef&& other) : mObject(other.mObject) {
            other.mObject = nullptr;
        }

        ~ObjectRef() {
            if (mObject != nullptr) {
                mObject->removeReference();
            }
        }

        ObjectRef& operator=(const ObjectRef& other) {
            if (&other != this) {
                mObject = other.mObject;
                mObject->addReference();
            }

            return *this;
        }

        ObjectRef& operator=(ObjectRef&& other) noexcept {
            mObject = other.mObject;
            other.mObject = nullptr;

            return *this;
        }

        void release() {
            if (mObject != nullptr) {
                mObject->removeReference();
                mObject = nullptr;
            }
        }

        operator Object*() {
            return mObject;
        }

        Object* operator->() {
            return mObject;
        }

        bool operator==(const ObjectRef& other) const {
            return *this == other.mObject;
        }

        bool operator==(Object* other) const {
            return mObject == other;
        }

        bool operator!=(const ObjectRef& other) const {
            return *this != other.mObject;
        }

        bool operator!=(Object* other) const {
            return mObject != other;
        }

        operator bool() const {
            return mObject != nullptr;
        }

    private:
        Object* mObject;
    };

    struct Array {
        Array(Class* clas, Int size); // use the artificial array class. NOT a base class

        Object object;
        const Int size;
    };

    template<typename T>
    constexpr T* Object::getArrayElements() {
        return std::launder(reinterpret_cast<T*>(reinterpret_cast<u8*>(this) + sizeof(Array)));
    }

    Object* AllocObject(Class* clas);
    Object* AllocArray(Class* clas, Int size);
}

#endif //JESUSVM_OBJECT_H
