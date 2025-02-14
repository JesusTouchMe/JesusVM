#ifndef JESUSVM_TYPE_H
#define JESUSVM_TYPE_H

#include "types.h"

#include <string_view>
#include <vector>

#undef VOID
#undef BOOL

namespace JesusVM {
    class Function;
    class Field;

    enum class Type {
        VOID,
        REFERENCE,
        HANDLE,
        BYTE,
        SHORT,
        INT,
        LONG,
        CHAR,
        FLOAT,
        DOUBLE,
        BOOL,

        TYPE_COUNT
    };

#ifdef JESUSVM_CONFIG_32BIT
    constexpr u32 REFERENCE_SIZE = 4;
    constexpr u32 HANDLE_SIZE = 4;
#else
    constexpr u32 REFERENCE_SIZE = 8;
    constexpr u32 HANDLE_SIZE = 8;
#endif

	class TypeInfo {
    public:
        TypeInfo() {}
        TypeInfo(std::string_view name) : mType(Type::REFERENCE), mClassName(name) {}

        Type getInternalType() const { return mType; }
        std::string_view getClassName() const { return mClassName; }

        bool parse(std::string_view descriptor, u32* index);

        bool is64Bit() const {
#ifdef JESUSVM_CONFIG_32BIT
            return mType == Type::LONG || mType == Type::DOUBLE;
#else
            return mType == Type::LONG || mType == Type::REFERENCE || mType == Type::DOUBLE || mType == Type::HANDLE;
#endif
        }

        bool isFloat() const { return mType == Type::FLOAT || mType == Type::DOUBLE; }

        u32 getSlotCount() const { // stack slot count. each slot is 4 bytes
            if (is64Bit())
                return 2;
            return 1;
        }

        u32 getByteSize() const {
            switch (mType) {
                case Type::VOID:
                    return 0;
                case Type::REFERENCE:
                    return REFERENCE_SIZE;
                case Type::HANDLE:
                    return HANDLE_SIZE;
                case Type::BYTE:
                case Type::CHAR:
                case Type::BOOL:
                    return 1;
                case Type::SHORT:
                    return 2;
                case Type::INT:
                case Type::FLOAT:
                    return 4;
                case Type::LONG:
                case Type::DOUBLE:
                    return 8;

                default:
                    //TODO: error?
                    return 0;
            }
        }

        bool isPrimitiveArrayType() const {
            u32 index = 0;
            if (mClassName[index] != '[') {
                return false;
            }

            while (mClassName[index] == '[') {
                index++;
            }

            return mClassName[index] != 'R';
        }

        bool isArray() {
            return mClassName[0] == '[';
        }

    private:
        Type mType;
        std::string_view mClassName; // descriptor, but also allows searching up the class name in the vm.
	};

    Type StringToType(std::string_view descriptor);

    bool ParseFunctionType(Function* function);
    bool ParseFieldType(Field* field);
}

#endif // JESUSVM_TYPE_H
