#pragma once

#include "JesusVM/type/Type.h"

namespace JesusVM {
	class PrimitiveType : public Type {
	public:
		PrimitiveType(std::string_view name, std::string_view descriptor, u32 sizeInBits);

		u32 getSizeInBits() const;

	private:
		u32 mSizeInBits;
	};
}