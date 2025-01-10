#pragma once

#include "types.h"

#include "JesusVM/Module.h"

#include "JesusVM/executors/Thread.h"

#include "JesusVM/type/TypeSystem.h"

#include <memory>
#include <vector>

namespace JesusVM {
	class JesusVM {
	friend class Thread;
	public:
		JesusVM();

		void start(Function* main); // should be called from the main thread
		void stop();

		TypeSystem& getTypeSystem();

		Module* getModule(std::string_view name);
		VThread* getAvailableThread();

		void addModule(std::unique_ptr<Module> module);
		void addThread(std::unique_ptr<Thread> thread);

	private:
		TypeSystem mTypeSystem;

		std::vector<std::unique_ptr<Module>> mModules;
		std::vector<std::unique_ptr<Thread>> mThreads;

		Thread* mMainThread;
	};
}