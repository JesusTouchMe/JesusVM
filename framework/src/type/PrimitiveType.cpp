#include "JesusVM/type/PrimitiveType.h"

namespace JesusVM {
	static Type::Special GetSpecialType(u32 bits) {
		switch (bits) {
			case 8:
				return Type::PRIMITIVE_8;
			case 16:
				return Type::PRIMITIVE_16;
			case 32:
				return Type::PRIMITIVE_32;
			case 64:
				return Type::PRIMITIVE_64;

			default:
				return Type::VOID;
		}
	}

	PrimitiveType::PrimitiveType(std::string_view name, std::string_view descriptor, u32 sizeInBits)
		: Type(name, descriptor, GetSpecialType(sizeInBits))
		, mSizeInBits(sizeInBits) { }

	u32 PrimitiveType::getSizeInBits() const {
		return mSizeInBits;
	}
}