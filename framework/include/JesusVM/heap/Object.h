#ifndef JESUSVM_OBJECT_H
#define JESUSVM_OBJECT_H

#include "JesusVM/JesusNative.h"

#include "JesusVM/heap/Class.h"

#include <atomic>

namespace JesusVM {
    struct Array;

    class Object {
    public:
        static Object* Alloc();

        Class* getClass();
        u64 getReferenceCount() const;

        void addReference();
        void removeReference();

        Boolean getBoolean(Field* field) const;
        Byte getByte(Field* field) const;
        Short getShort(Field* field) const;
        Int getInt(Field* field) const;
        Long getLong(Field* field) const;
        Object* getObject(Field* field) const;

    private:
        Object();
        ~Object();

        u8* getFieldsBuffer();

        Class* mClass;

        std::atomic<u64> mRefCount;
    };

    struct Array {
        Object object;
        u64 size;
    };
}

#endif //JESUSVM_OBJECT_H
