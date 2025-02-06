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
        u64 getReferenceCount() const;

        Array* toArray();
        u64 getArrayLength();

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

        std::atomic<u64> mRefCount;
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
