#include "JesusVM/Function.h"

#include "JesusVM/heap/Field.h"

#include "JesusVM/type/Type.h"

#undef BOOL
#undef VOID

namespace JesusVM {
    static bool ParseClassType(std::string_view descriptor, u32* index) {
        do {
            if (*index >= descriptor.length()) {
                return true;
            }

            (*index)++;
        } while (descriptor[*index] != ';');

        (*index)++;
        return false;
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

        switch (descriptor[*index]) {
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

    u32 GetTypeSize(Type type) {
        switch (type) {
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

    constexpr Type MakeTypeMap(size_t index) {
        switch (index) {
            case T_BYTE:    return Type::BYTE;
            case T_SHORT:   return Type::SHORT;
            case T_INT:     return Type::INT;
            case T_LONG:    return Type::LONG;
            case T_CHAR:    return Type::CHAR;
            case T_FLOAT:   return Type::FLOAT;
            case T_DOUBLE:  return Type::DOUBLE;
            case T_BOOL:    return Type::BOOL;
            case T_HANDLE:  return Type::HANDLE;
            default:
               std::cout << "Unreachable\n";
               std::exit(1);
        }
    }

    template <size_t... Indices>
    constexpr auto MakeTypeArray(std::index_sequence<Indices...>) {
        return std::array<Type, sizeof...(Indices)>{ MakeTypeMap(Indices)... };
    }

    Type CodeTypeToType(u8 type) {
        static constexpr auto map = MakeTypeArray(std::make_index_sequence<T_MAX>{});

        if (type >= sizeof(map) / sizeof(map[0])) {
            std::cout << "error: provided code type id does not match any known type id\n";
            std::exit(1);
        }

        return map[type];
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

    bool ParseFieldType(Field* field) {
        std::string_view descriptor = field->mDescriptor;
        u32 index = 0;

        return field->mType.parse(descriptor, &index);
    }
}