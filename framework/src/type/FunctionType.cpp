#include "JesusVM/util/StringPool.h"
#include "JesusVM/type/FunctionType.h"

namespace JesusVM {
	static std::string_view GetTypeName(Type* returnType, const std::vector<Type*>& argumentTypes) {
		std::string name;

		name += returnType->getName();
		name += '(';

		if (!argumentTypes.empty()) {
			for (auto i = 0; i < argumentTypes.size() - 1; i++) {
				name += argumentTypes[i]->getName();
				name += ", ";
			}
			name += argumentTypes.back()->getName();
		}

		name += ')';

		return StringPool::Intern(name);
	}

	static std::string_view GetTypeDescriptor(Type* returnType, const std::vector<Type*>& argumentTypes) {
		std::string descriptor(1, '(');

		for (auto type : argumentTypes) {
			descriptor += type->getDescriptor();
		}
		descriptor += ')';

		descriptor += returnType->getDescriptor();

		return StringPool::Intern(descriptor);
	}

	FunctionType::FunctionType(Type* returnType, std::vector<Type*> argumentTypes)
		: Type(GetTypeName(returnType, argumentTypes), GetTypeDescriptor(returnType, argumentTypes), Type::FUNCTION)
		, mReturnType(returnType)
		, mArgumentTypes(std::move(argumentTypes)) { }

	Type* FunctionType::getReturnType() const {
		return mReturnType;
	}

	const std::vector<Type*>& FunctionType::getArgumentTypes() const {
		return mArgumentTypes;
	}
}