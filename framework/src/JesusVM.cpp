#include "JesusVM/JesusVM.h"

#include "JesusVM/runtime/std/Primitives.h"

#include <algorithm>

namespace JesusVM {
	JesusVM::JesusVM()
		: mMainThread(nullptr) {}

	void JesusVM::start(Function* main) {
		if (mMainThread == nullptr) {
			std::cout << "Major problem. The VM has no defined main thread. TODO: proper error\n";
			std::exit(1);
		}

        mMainThread->setFunction(main);
	}

	void JesusVM::stop() {
        Threading::WaitForAllThreads();
	}

    VMContext JesusVM::getContext() {
        return reinterpret_cast<VMContext>(&mContext);
    }

    std::string_view GetStringData(Object* object) {
        if (object->getClass() != rt::std::Primitives::String::self) {
            std::cout << "insert type mismatch error\n";
            std::exit(1);
        }

        Object* data = object->getObjectWeak(rt::std::Primitives::String::data);
        Char* cString = data->getArrayElements<Char>();
        Int length = data->getArrayLength();

        return { cString, static_cast<std::size_t>(length) };
    }
}