#include "JesusVM/JesusVM.h"

#include <algorithm>

namespace JesusVM {
	JesusVM::JesusVM()
		: mMainThread(nullptr) {}

	void JesusVM::start(Function* main) {
		if (mMainThread == nullptr) {
			std::cout << "Major problem. The VM has no defined main thread. TODO: proper error\n";
			std::exit(1);
		}

		if (main->isNative()) {
			//TODO: thread system supporting native execution

			auto ptr = reinterpret_cast<NativeFunctionPtr<void>>(main->getEntry());
			ptr(nullptr, nullptr);
		} else {
			VThread* runner = mMainThread->getAvailableThread();
			if (runner == nullptr) {
				std::unique_ptr<VThread> ptr = std::make_unique<VThread>(*this);
				runner = ptr.get();
				mMainThread->addVThread(std::move(ptr));
			}

			mMainThread->mMainVThread = runner;
			runner->executeFunction(main);

			mMainThread->run();
		}
	}

	void JesusVM::stop() {
		for (auto& thread : mThreads) {
			thread->stop();
		}
	}

    VMContext JesusVM::getContext() {
        return reinterpret_cast<VMContext>(&mContext);
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
		if (mThreads.empty()) return nullptr;

		Thread* least = mThreads.front().get();
		for (auto& thread : mThreads) {
			VThread* vThread = thread->getAvailableThread();
			if (vThread != nullptr) {
				return vThread;
			}

			if (thread->getThreadCount() < least->getThreadCount()) {
				least = thread.get();
			}
		}

		std::unique_ptr<VThread> vThread = std::make_unique<VThread>(*this);
		VThread* result = vThread.get();

		least->addVThread(std::move(vThread));

		return result;
	}

	void JesusVM::addModule(std::unique_ptr<Module> module) {
		if (module != nullptr) {
			mModules.push_back(std::move(module));
		}
	}

	void JesusVM::addThread(std::unique_ptr<Thread> thread) {
		if (thread != nullptr) {
			mThreads.push_back(std::move(thread));
		}
	}
}