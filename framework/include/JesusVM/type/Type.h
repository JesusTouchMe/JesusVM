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

    enum CodeType : u8 { // these are the type IDs used in bytecode for newarray (and maybe more) instructions
        T_BYTE,
        T_SHORT,
        T_INT,
        T_LONG,
        T_CHAR,
        T_FLOAT,
        T_DOUBLE,
        T_BOOL,

        T_MAX,
    };

    enum class Type : u8 {
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

    constexpr u32 REFERENCE_SIZE = 8;
    constexpr u32 HANDLE_SIZE = 8;

    u32 GetTypeSize(Type type);

	class TypeInfo {
    public:
        TypeInfo() {}
        TypeInfo(std::string_view name) : mType(Type::REFERENCE), mClassName(name) {}

        Type getInternalType() const { return mType; }
        std::string_view getClassName() const { return mClassName; }

        bool parse(std::string_view descriptor, u32* index);

        bool is64Bit() const {
            return mType == Type::LONG || mType == Type::REFERENCE || mType == Type::DOUBLE || mType == Type::HANDLE;
        }

        bool isFloat() const { return mType == Type::FLOAT || mType == Type::DOUBLE; }

        u32 getSlotCount() const { // stack slot count. each slot is 4 bytes
            if (is64Bit())
                return 2;
            return 1;
        }

        u32 getByteSize() const {
            return GetTypeSize(mType);
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

    Type CodeTypeToType(u8 type);

    Type StringToType(std::string_view descriptor);

    bool ParseFunctionType(Function* function);
    bool ParseFieldType(Field* field);
}

#endif // JESUSVM_TYPE_H
