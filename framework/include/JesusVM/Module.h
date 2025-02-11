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
#include <unordered_map>

namespace JesusVM {
    class JesusVM;

    // for friends. import Linker.h and do NOT use these
    namespace Linker {
        static Module* LoadModuleWithBootstrap(std::string_view);
        static Class* FindClass(std::unique_lock<std::mutex>&, Module* module, std::string_view name);
        Class* LoadClass(Module* module, std::string_view name);
    }

	class Module {
    friend Module* Linker::LoadModuleWithBootstrap(std::string_view name);
    friend Class* Linker::FindClass(std::unique_lock<std::mutex>&, Module* module, std::string_view name);
    friend Class* Linker::LoadClass(Module* module, std::string_view name);
	public:
		Module(JesusVM& vm, Object* linker, moduleweb_module_info* info);
        ~Module();

        const moduleweb_module_info* getInfo();

        JesusVM& getVM() const;

		std::string_view getName() const;
		ConstPool& getConstPool();

        Class* findClass(std::string_view name);
		Class* getClass(std::string_view name);
		Function* getFunction(std::string_view name, std::string_view descriptor);
        Function* getFunction(ConstantName* name);

	private:
        moduleweb_module_info* mInfo;

        JesusVM& mVM;
        Object* mLinker;

		std::string_view mName;

		ConstPool mConstPool;
		
		std::unordered_map<std::string_view, std::unique_ptr<Class>> mClasses;
		std::vector<Function> mFunctions;
	};
}

#endif // JESUSVM_MODULE_H