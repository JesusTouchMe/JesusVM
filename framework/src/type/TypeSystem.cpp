#include "JesusVM/JesusVM.h"

#include "JesusVM/type/FunctionType.h"
#include "JesusVM/type/PrimitiveType.h"
#include "JesusVM/type/ReferenceType.h"
#include "JesusVM/type/TypeSystem.h"
#include "JesusVM/type/VoidType.h"

namespace JesusVM {
	TypeSystem::TypeSystem(JesusVM& vm)
		: mVM(vm) {
		mTypes["V"] = std::make_unique<VoidType>();
		mTypes["B"] = std::make_unique<PrimitiveType>("byte", "B", 8);
		mTypes["S"] = std::make_unique<PrimitiveType>("short", "S", 16);
		mTypes["I"] = std::make_unique<PrimitiveType>("int", "I", 32);
		mTypes["L"] = std::make_unique<PrimitiveType>("long", "L", 64);
	}

	Type* TypeSystem::getType(std::string_view descriptor) {
		auto it = mTypes.find(descriptor);
		if (it == mTypes.end()) {
			if (descriptor.starts_with("(")) { // function descriptor
				return parseFunctionType(descriptor);
			} else if (descriptor.starts_with("R")) { // reference type
				return parseReferenceType(descriptor);
			}
		}

		return it->second.get();
	}

	Type* TypeSystem::parseReferenceType(std::string_view descriptor) {
		std::string_view fullName = descriptor.substr(1, descriptor.size() - 2);

		auto foundIndex = fullName.find_last_of('/');
		if (foundIndex == std::string_view::npos) {
			return nullptr;
		}

		std::string_view moduleName = fullName.substr(0, foundIndex);
		std::string_view className = fullName.substr(foundIndex + 1);

		Module* module = mVM.getModule(moduleName);
		Class* clas = module->getClass(className);

		auto referenceType = std::make_unique<ReferenceType>(clas);
		Type* result = referenceType.get();

		mTypes.emplace(descriptor, std::move(referenceType));

		return result;
	}

	Type* TypeSystem::parseFunctionType(std::string_view descriptor) {
		u64 pos = 0;

		if (descriptor[pos] != '(') return nullptr;
		pos++;

		std::vector<Type*> arguments;

		while (descriptor[pos] != ')') {
			if (descriptor[pos] == 'R') {
				u64 start = pos;
				while (descriptor[pos] != ';') {
					pos++;
				}

				std::string_view refDesc = descriptor.substr(start, pos - start + 1);
				arguments.push_back(getType(refDesc));
				pos++;
			} else {
				arguments.push_back(getType(descriptor.substr(pos, 1)));
				pos++;
			}
		}

		std::string_view returnDesc = descriptor.substr(pos + 1);
		Type* returnType = getType(returnDesc);

		auto functionType = std::make_unique<FunctionType>(returnType, std::move(arguments));
		Type* result = functionType.get();

		mTypes.emplace(descriptor, std::move(functionType));

		return result;
	}
}