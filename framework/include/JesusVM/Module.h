#ifndef JESUSVM_MODULE_H
#define JESUSVM_MODULE_H

#include "types.h"

#include "JesusVM/Function.h"

#include "JesusVM/constpool/ConstPool.h"

#include "JesusVM/heap/Class.h"

#include <memory>
#include <string_view>

namespace JesusVM {
	class Module {
	public:
		Module(std::string_view name, u32 constCount, std::vector<Class> classes, std::vector<Function> functions);

		std::string_view getName() const;
		ConstPool& getConstPool();

		Class* getClass(std::string_view name);
		Function* getFunction(std::string_view descriptor);

	private:
		std::string_view mName;

		ConstPool mConstPool;
		
		std::vector<Class> mClasses;
		std::vector<Function> mFunctions;
	};
}

#endif // JESUSVM_MODULE_H