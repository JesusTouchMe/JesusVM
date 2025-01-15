#ifndef JESUSVM_FUNCTIONTYPE_H
#define JESUSVM_FUNCTIONTYPE_H

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

#endif // JESUSVM_FUNCTIONTYPE_H