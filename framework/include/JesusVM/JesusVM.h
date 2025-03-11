#ifndef JESUSVM_JESUSVM_H
#define JESUSVM_JESUSVM_H

#include "types.h"

#include "JesusVM/Linker.h"
#include "JesusVM/Module.h"

#include "JesusVM/executors/Thread.h"

#include "JesusVM/heap/Object.h"

#include <memory>
#include <vector>

#define ALIGN(x,a) (((x)+(a)-1)&~((a)-1))

namespace JesusVM {
    struct Context {

    };

	class JesusVM {
	friend class Thread;
	public:
		JesusVM();

		void start(Function* main); // should be called from the main thread
		void stop();

        VMContext getContext();

	private:
		Thread* mMainThread;

        Context mContext;

        bool mPreloadFinished; //TODO: implement
	};

    std::string_view GetStringData(Object* object);
}

#endif // JESUSVM_JESUSVM_H
