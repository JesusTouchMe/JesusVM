#include "JesusVM/Module.h"

#include "JesusVM/util/StringPool.h"

#include "JesusVM/type/ReferenceType.h"

namespace JesusVM {
	static std::string_view GetClassDescriptor(Class* clas) {
		std::string str(1, 'R');

		str += clas->getModule()->getName();
		str += '/';
		str += clas->getName();
		str += ';';

		return StringPool::Intern(str);
	}

	ReferenceType::ReferenceType(Class* clas)
		: Type(clas->getName(), GetClassDescriptor(clas), Type::REFERENCE)
		, mClass(clas) {}

	Class* ReferenceType::getClass() const {
		return mClass;
	}
}