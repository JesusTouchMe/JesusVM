#ifndef JESUSVM_MODULE_H
#define JESUSVM_MODULE_H

#include "types.h"

#include "JesusVM/Function.h"

#include "JesusVM/constpool/ConstantName.h"
#include "JesusVM/constpool/ConstPool.h"

#include "JesusVM/heap/Class.h"

#include "moduleweb/module_info.h"

#include <memory>
#include <string_view>

namespace JesusVM {
    class JesusVM;

	class Module {
	public:
		Module(JesusVM& vm, moduleweb_module_info* info);
        ~Module();

        const moduleweb_module_info* getInfo();

        JesusVM& getVM() const;

		std::string_view getName() const;
		ConstPool& getConstPool();

		Class* getClass(std::string_view name);
		Function* getFunction(std::string_view name, std::string_view descriptor);
        Function* getFunction(ConstantName* name);

	private:
        moduleweb_module_info* mModulewebInfo;

        JesusVM& mVM;

		std::string_view mName;

		ConstPool mConstPool;
		
		std::vector<Class> mClasses;
		std::vector<Function> mFunctions;
	};
}

#endif // JESUSVM_MODULE_H