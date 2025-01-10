#include "JesusVM/JesusVM.h"

namespace JesusVM {
	JesusVM::JesusVM()
		: mTypeSystem(*this)
		, mMainThread(nullptr) {}

	void JesusVM::start(Function* main) {
		if (mMainThread == nullptr) {
			std::cout << "Major problem. The VM has no defined main thread. TODO: proper error\n";
			std::exit(1);
		}

		VThread* runner = mMainThread->getAvailableThread();
		if (runner == nullptr) {
			std::unique_ptr<VThread> ptr = std::make_unique<VThread>();
			runner = ptr.get();
			mMainThread->addVThread(std::move(ptr));
		}

		runner->executeFunction(main);

		mMainThread->run();
	}

	void JesusVM::stop() {
		mMainThread->stop();
	}

	TypeSystem& JesusVM::getTypeSystem() {
		return mTypeSystem;
	}

	Module* JesusVM::getModule(std::string_view name) {
		auto it = std::find_if(mModules.rbegin(), mModules.rend(), [name](auto& module) { return module->getName() == name; });
		if (it != mModules.rend()) return (*it).get();

		return nullptr;
	}

	VThread* JesusVM::getAvailableThread() {
		Thread* least = nullptr;
		for (auto& thread : mThreads) {
			VThread* vThread = thread->getAvailableThread();
			if (vThread != nullptr) {
				return vThread;
			}

			if (least == nullptr || thread->getThreadCount() < least->getThreadCount()) {
				least = thread.get();
			}
		}

		if (least == nullptr) return nullptr; // there are no system threads

		std::unique_ptr<VThread> vThread = std::make_unique<VThread>();
		VThread* result = vThread.get();

		least->addVThread(std::move(vThread));

		return result;
	}

	void JesusVM::addModule(std::unique_ptr<Module> module) {
		mModules.push_back(std::move(module));
	}

	void JesusVM::addThread(std::unique_ptr<Thread> thread) {
		mThreads.push_back(std::move(thread));
	}
}