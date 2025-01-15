#ifndef JESUSVM_TYPESYSTEM_H
#define JESUSVM_TYPESYSTEM_H

#include "JesusVM/heap/Class.h"

#include "JesusVM/type/Type.h"

#include <memory>
#include <unordered_map>

namespace JesusVM {
	class JesusVM;

	class TypeSystem {
	public:
		TypeSystem(JesusVM& vm);

		Type* getType(std::string_view descriptor);

		Type* getVoidType();
		Type* getPrimitiveType(u32 size);
		Type* getReferenceType(Class* clas);
		Type* getFunctionType(Type* returnType, std::vector<Type*> argumentTypes);

	private:
		JesusVM& mVM;

		std::unordered_map<std::string_view, std::unique_ptr<Type>> mTypes;

		Type* parseReferenceType(std::string_view descriptor);
		Type* parseFunctionType(std::string_view descriptor);
	};
}

#endif // JESUSVM_TYPESYSTEM_H