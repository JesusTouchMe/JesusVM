#include "JesusVM/heap/Object.h"

#include <cstdlib>

namespace JesusVM {
    u8* Object::getFieldsBuffer() {
        return reinterpret_cast<u8*>(std::launder(this + 1));
    }

    Array::Array(Class* clas, Int size)
        : object(clas)
        , size(size) {}

    Object* AllocObject(Class* clas) {
        return nullptr;
    }

    Object* AllocArray(Class* clas, Int size) {
        u64 totalSize = sizeof(Array) + sizeof(Object*) * size;
#ifdef PLATFORM_WINDOWS
        u8* memory = static_cast<u8*>(_aligned_malloc(totalSize, alignof(Array)));
#else
        u8* memory = static_cast<u8*>(std::aligned_alloc(alignof(Array), totalSize));
#endif

        if (memory == nullptr) {
            return nullptr; //TODO: out-of-memory error
        }

        Array* array = new(memory) Array(clas, size);

        for (int i = 0; i < size; i++) {
            new(reinterpret_cast<char*>(array + 1) + i * sizeof(Object*)) Object*(nullptr);
        }

        Object** elements = array->object.getArrayElements<Object*>();
        for (Int i = 0; i < size; i++) {
            elements[i] = nullptr;
        }

        return &array->object;
    }
}