#pragma once

#include "JesusVM/type/Type.h"

namespace JesusVM {
	class FunctionType : public Type {
	public:
		FunctionType(Type* returnType, std::vector<Type*> argumentTypes);

		Type* getReturnType() const;
		const std::vector<Type*>& getArgumentTypes() const;

	private:
		Type* mReturnType;
		std::vector<Type*> mArgumentTypes;
	};
}