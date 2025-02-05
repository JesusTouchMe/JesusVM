#include "JesusVM/Function.h"

#include "JesusVM/heap/Field.h"

#include "JesusVM/type/Type.h"

namespace JesusVM {
    static bool ParseClassType(std::string_view descriptor, u32* index) {

    }

    static bool ParseArrayBaseType(std::string_view descriptor, u32* index) {
        switch (descriptor[*index]) {
            case '[':
                (*index)++;
                return ParseArrayBaseType(descriptor, index);

            case 'R':
                (*index)++;
                return ParseClassType(descriptor, index);

            case 'H':
            case 'V':
            case 'Z':
            case 'B':
            case 'S':
            case 'I':
            case 'L':
            case 'C':
            //case 'F':
            //case 'D':
                (*index)++;
                return false;

            default:
                return true;
        }
    }

    bool TypeInfo::parse(std::string_view descriptor, u32* index) {
        u32 nameStart;

        switch (descriptor[0]) {
            case '[':
                nameStart = (*index)++;
                if (ParseArrayBaseType(descriptor, index)) {
                    return true;
                }

                mType = Type::REFERENCE;
                mClassName = descriptor.substr(nameStart, *index - nameStart);

                return false;

            case 'R':
                nameStart = ++(*index);
                if (ParseClassType(descriptor, index)) {
                    return true;
                }

                mType = Type::REFERENCE;
                mClassName = descriptor.substr(nameStart, *index - nameStart - 1);

                return false;

            case 'H':
            case 'V':
            case 'Z':
            case 'B':
            case 'S':
            case 'I':
            case 'L':
            case 'C':
            //case 'F':
            //case 'D':
                mClassName = descriptor.substr(*index, 1);
                mType = StringToType(mClassName);

                (*index)++;

                return false;

            default:
                return true;
        }
    }

    Type StringToType(std::string_view descriptor) {
        switch (descriptor[0]) {
            case 'V':
                return Type::VOID;
            case 'Z':
                return Type::BOOL;
            case 'B':
                return Type::BYTE;
            case 'S':
                return Type::SHORT;
            case 'I':
                return Type::INT;
            case 'L':
                return Type::LONG;
            case 'C':
                return Type::CHAR;
            case 'F':
                return Type::FLOAT;
            case 'D':
                return Type::DOUBLE;
            case 'H':
                return Type::HANDLE;

            default:
                return Type::REFERENCE;
        }
    }

    bool ParseFunctionType(Function* function) {
        std::string_view descriptor = function->mDescriptor;
        u32 index = 0;

        if (descriptor[index++] != '(')
            return true;

        while (descriptor[index] != ')') {
            function->mArgumentTypes.emplace_back();
            TypeInfo& arg = function->mArgumentTypes.back();

            if (arg.parse(descriptor, &index)) {
                return true;
            }
        }

        index++;

        if (function->mReturnType.parse(descriptor, &index)) {
            return true;
        }

        if (index < descriptor.length()) {
            return true;
        }

        return false;
    }

    bool ParseFieldType(Field* field) {
        std::string_view descriptor = field->mDescriptor;
        u32 index = 0;

        return field->mType.parse(descriptor, &index);
    }
}