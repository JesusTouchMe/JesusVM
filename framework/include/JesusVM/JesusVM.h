#ifndef JESUSVM_JESUSVM_H
#define JESUSVM_JESUSVM_H

#include "types.h"

#include "JesusVM/Module.h"

#include "JesusVM/executors/Thread.h"

#include "JesusVM/type/TypeSystem.h"

#include <memory>
#include <vector>

namespace JesusVM {
    struct Context {

    };

	class JesusVM {
	friend class Thread;
	public:
		JesusVM();

		void start(Function* main); // should be called from the main thread
		void stop();

		TypeSystem& getTypeSystem();
        VMContext getContext();

		Module* getModule(std::string_view name);
		VThread* getAvailableThread();

		void addModule(std::unique_ptr<Module> module);
		void addThread(std::unique_ptr<Thread> thread);

	private:
		std::vector<std::unique_ptr<Module>> mModules;
		std::vector<std::unique_ptr<Thread>> mThreads;

		Thread* mMainThread;

        Context mContext;
	};
}

#endif // JESUSVM_JESUSVM_H
