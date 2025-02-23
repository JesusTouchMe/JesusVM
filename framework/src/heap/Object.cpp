#include "JesusVM/Linker.h"

#include "JesusVM/heap/Object.h"

#include "JesusVM/runtime/vm/System.h"

#include <cstdlib>

namespace JesusVM {
    u8* Object::getFieldsBuffer() {
        return reinterpret_cast<u8*>(std::launder(this + 1));
    }

    Array::Array(Class* clas, Int size)
        : object(clas)
        , size(size) {}

    ObjectRef AllocObject(Class* clas) {
        return nullptr;
    }

    ObjectRef AllocArray(Class* clas, Int size) {
        u64 totalSize = sizeof(Array) + sizeof(Object*) * size;
#ifdef PLATFORM_WINDOWS
        u8* memory = static_cast<u8*>(_aligned_malloc(totalSize, alignof(Array)));
#else
        u8* memory = static_cast<u8*>(std::aligned_alloc(alignof(Array), totalSize));
#endif

        if (memory == nullptr) {
            return nullptr; //TODO: out-of-memory error
        }

        auto array = new(memory) Array(clas, size);

        auto elements = array->object.getArrayElements<Object*>();
        for (Int i = 0; i < size; i++) {
            elements[i] = nullptr;
        }

        return &array->object;
    }

    ObjectRef AllocPrimitiveArray(u8 typeId, Int size) {
        Type type = CodeTypeToType(typeId);

        u64 totalSize = sizeof(Array) + GetTypeSize(type) * size;
#ifdef PLATFORM_WINDOWS
        u8* memory = static_cast<u8*>(_aligned_malloc(totalSize, alignof(Array)));
#else
        u8* memory = static_cast<u8*>(std::aligned_alloc(alignof(Array), totalSize));
#endif

        if (memory == nullptr) {
            return nullptr; //TODO: out-of-memory error
        }
        
        Class* clas;

        switch (typeId) {
            case T_BYTE:
                clas = rt::vm::System::byteArray;
                break;
            case T_SHORT:
                clas = rt::vm::System::shortArray;
                break;
            case T_INT:
                clas = rt::vm::System::intArray;
                break;
            case T_LONG:
                clas = rt::vm::System::longArray;
                break;
            case T_CHAR:
                clas = rt::vm::System::charArray;
                break;
            case T_FLOAT:
                clas = rt::vm::System::floatArray;
                break;
            case T_DOUBLE:
                clas = rt::vm::System::doubleArray;
                break;
            case T_BOOL:
                clas = rt::vm::System::boolArray;
                break;

            default:
                // it will never reach here since CodeTypeToType() error and crash way before
                std::exit(1);
        }

        if (clas == nullptr) {
            std::cout << "error: what?\n"; // i really hope this is never reached
            std::exit(1);
        }

        auto array = new(memory) Array(clas, size);

        return &array->object;
    }
}