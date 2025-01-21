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

    }

    Object* AllocArray(Class* clas, Int size) {
        u8* stupidLanguage = new(static_cast<std::align_val_t>(alignof(Array))) u8[sizeof(Array) + sizeof(Object*) * size];
        new(stupidLanguage) Array(clas, size);
        Array* array = std::launder(reinterpret_cast<Array*>(stupidLanguage));

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