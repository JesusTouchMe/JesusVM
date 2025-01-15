#ifndef JESUSVM_TYPE_H
#define JESUSVM_TYPE_H

#include "types.h"

#include <string_view>
#include <vector>

namespace JesusVM {
	class JesusVM;

	class Type {
	public:
		enum Special {
			VOID,
			PRIMITIVE_8,
			PRIMITIVE_16,
			PRIMITIVE_32,
			PRIMITIVE_64,
			REFERENCE,
			FUNCTION,
		};

		virtual ~Type() { }
		
		std::string_view getName() const { return mName; }
		std::string_view getDescriptor() const { return mDescriptor; }

		bool isVoid() const { return mSpecial == VOID; }
		bool isPrimitive8() const { return mSpecial == PRIMITIVE_8; }
		bool isPrimitive16() const { return mSpecial == PRIMITIVE_16; }
		bool isPrimitive32() const { return mSpecial == PRIMITIVE_32; }
		bool isPrimitive64() const { return mSpecial == PRIMITIVE_64; }
		bool isPrimitive() const { return mSpecial >= PRIMITIVE_8 && mSpecial <= PRIMITIVE_64; }
		bool isReference() const { return mSpecial == REFERENCE; }
		bool isFunction() const { return mSpecial == FUNCTION; }

	protected:
		Type(std::string_view name, std::string_view descriptor, Special special)
			: mName(name)
			, mDescriptor(descriptor)
			, mSpecial(special)
		{ }

		std::string_view mName;
		std::string_view mDescriptor;
		Special mSpecial;
	};
}

#endif // JESUSVM_TYPE_H
