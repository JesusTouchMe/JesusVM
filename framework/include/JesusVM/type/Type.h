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
        T_HANDLE,

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

        bool isFloat() const { return mType == Type::FLOAT || mType == Type::DOUBLE; }

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

        bool isArray() const {
            return mClassName[0] == '[';
        }

    private:
        Type mType;
        std::string_view mClassName; // descriptor, but also allows searching up the class name in the vm.
	};

    Type CodeTypeToType(u8 type);

    Type StringToType(std::string_view descriptor);

    bool ParseFieldType(Field* field);

    template<class T>
    bool ParseFunctionType(T* function) {
        std::string_view descriptor = function->mDescriptor;
        u32 index = 0;

        if (descriptor[index++] != '(')
            return true;

        while (index < descriptor.length() && descriptor[index] != ')') {
            function->mArgumentTypes.emplace_back();
            TypeInfo& arg = function->mArgumentTypes.back();

            if (arg.parse(descriptor, &index)) {
                return true;
            }
        }

        if (index >= descriptor.length() || descriptor[index++] != ')') {
            return true;
        }

        if (function->mReturnType.parse(descriptor, &index)) {
            return true;
        }

        if (index < descriptor.length()) {
            return true;
        }

        return index != descriptor.length();
    }
}

#endif // JESUSVM_TYPE_H
