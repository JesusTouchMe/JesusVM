#ifndef JESUSVM_OBJECT_H
#define JESUSVM_OBJECT_H

#include "JesusVM/JesusNative.h"

#include "JesusVM/heap/Class.h"

#include "JesusVM/type/Type.h"

#include <atomic>

namespace JesusVM {
    struct Array;

    class ObjectRef;

    class Object {
    friend struct Array;
    friend ObjectRef AllocObject(Class* clas);
    friend ObjectRef AllocArray(Class* clas, Int size);
    public:
        Class* getClass();
        i32 getReferenceCount() const;

        constexpr Array* toArray();

        Int getArrayLength();

        bool isInstance(Class* clas);

        void addReference();
        void removeReference();

        void nullCheck();

        u8* getFieldsBuffer();
        const u8* getFieldsBuffer() const;

        template<typename T>
        constexpr T* getArrayElements();

        Bool getBool(Field* field) const;
        Char getChar(Field* field) const;
        Byte getByte(Field* field) const;
        Short getShort(Field* field) const;
        Int getInt(Field* field) const;
        Long getLong(Field* field) const;
        // Float getFloat(Field* field) const;
        // Double getDouble(Field* field) const;
        Handle getHandle(Field* field) const;
        ObjectRef getObject(Field* field) const;
        Object* getObjectWeak(Field* field) const;

        void setBool(Field* field, Bool value);
        void setChar(Field* field, Char value);
        void setByte(Field* field, Byte value);
        void setShort(Field* field, Short value);
        void setInt(Field* field, Int value);
        void setLong(Field* field, Long value);
        // void setFloat(Field* field, Float value) const;
        // void setDouble(Field* field, Double value) const;
        void setHandle(Field* field, Handle value);
        void setObject(Field* field, Object* value);

    private:
        Object(Class* clas);

        Class* mClass;

        std::atomic<i32> mRefCount = 1;

        void freeThis();
    };

    class ObjectRef {
    public:
        ObjectRef(Object* object) : mObject(object) {
            if (mObject != nullptr)
                mObject->addReference();
        }

        // This constructor doesn't implicitly add its own reference, as it expects the caller to have already added a reference and is simply using this to transfer that.
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

        ObjectRef& operator=(Object* obj) {
            if (mObject != nullptr) {
                mObject->removeReference();
            }

            mObject = obj;
            mObject->addReference();

            return *this;
        }

        ObjectRef& operator=(std::nullptr_t) {
            if (mObject != nullptr) {
                mObject->removeReference();
            }

            mObject = nullptr;

            return *this;
        }

        Object* get() const {
            return mObject;
        }

        void release() {
            if (mObject != nullptr) {
                mObject->removeReference();
                mObject = nullptr;
            }
        }

        operator Object*() const {
            return mObject;
        }

        operator JObject() const {
            return reinterpret_cast<JObject>(mObject); // this is safe
        }

        Object* operator->() {
            return mObject;
        }

        bool operator==(const ObjectRef& other) const {
            return mObject == other.mObject;
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

        bool operator==(std::nullptr_t) const {
            return mObject == nullptr;
        }

        bool operator!=(std::nullptr_t) const {
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

    // the famous container_of macro (c++ edition) !!!
    // credit to https://stackoverflow.com/a/40851139

    template <class P, class M>
    std::size_t _offsetof(const M P::*member) {
        return (std::size_t) &(reinterpret_cast<P*>(0)->*member);
    }

    template<class P, class M>
    P* _container_of_impl(M* ptr, const M P::*member) {
        return (P*) ((char*) ptr - _offsetof(member));
    }

#   define container_of(ptr, type, member) _container_of_impl(ptr, &type::member)

    // inline Object methods for optimization
    constexpr Array* Object::toArray() {
        return container_of(this, Array, object);
    }

    Int Object::getArrayLength() {
        return toArray()->size;
    }

    template<typename T>
    constexpr T* Object::getArrayElements() {
        return std::launder(reinterpret_cast<T*>(reinterpret_cast<u8*>(this) + sizeof(Array)));
    }

    ObjectRef AllocObject(Class* clas);
    ObjectRef AllocArray(Class* clas, Int size);
    ObjectRef AllocPrimitiveArray(u8 typeId, Int size);
}

#endif //JESUSVM_OBJECT_H
