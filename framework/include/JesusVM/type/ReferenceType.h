#ifndef JESUSVM_REFERENCETYPE_H
#define JESUSVM_REFERENCETYPE_H

#include "JesusVM/heap/Class.h"

#include "JesusVM/type/Type.h"

namespace JesusVM {
	class ReferenceType : public Type {
	public:
		ReferenceType(Class* clas);

		Class* getClass() const;

	private:
		Class* mClass;
	};
}

#endif // JESUSVM_REFERENCETYPE_H
