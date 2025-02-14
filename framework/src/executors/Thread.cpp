#include "JesusVM/JesusVM.h"

#include "JesusVM/executors/Thread.h"

#include <algorithm>

namespace JesusVM {
	Thread::Thread(JesusVM& vm, Mode mode)
		: mInterrupted(false)
        , mState(State::NEW)
        , mVM(vm)
        , mMainThread(false) {
        setState(State::IDLE);
        setMode(mode);
    }

    Thread::State Thread::getState() const {
        return mState;
    }

    Thread::Mode Thread::getMode() const {
        return mMode;
    }

    std::thread::id Thread::getId() const {
        return mId;
    }

    void Thread::setState(Thread::State state) {
        mState = state;
        mCondition.notify_all();
    }

    void Thread::setMode(Thread::Mode mode) {
        std::unique_lock<std::mutex> lock(mMutex);

        mCondition.wait(lock, [this] {
            return mState == State::IDLE; // we can't change the threads execution mode if it's not idle
        });

        mMode = mode;

        switch (mode) {
            case Mode::SINGLE_EXECUTOR:
                mThreadMode.emplace<SingleExecutor>(Executor(mVM));
                break;

            case Mode::VTHREAD_EXECUTOR:
                mThreadMode.emplace<VThreadExecutor>();
                break;

            case Mode::NATIVE_EXECUTOR:
                mThreadMode.emplace<NativeExecutor>(nullptr);
                break;
        }
    }

    void Thread::setFunction(Function* function) {
        std::unique_lock<std::mutex> lock(mMutex);

        if (mMode != Mode::VTHREAD_EXECUTOR) {
            mCondition.wait(lock, [this] {
                return mState == State::IDLE; // we can't change the threads execution mode if it's not idle
            });
        }

        switch (mMode) {
            case Mode::SINGLE_EXECUTOR: {
                auto& executor = std::get<SingleExecutor>(mThreadMode);
                executor.executor.enterFunction(function);
                executor.executor.run();

                break;
            }

            case Mode::VTHREAD_EXECUTOR: {
                auto& executor = std::get<VThreadExecutor>(mThreadMode);
                auto thread = executor.vThreads.emplace_back(std::make_unique<VThread>(mVM)).get();

                thread->executeFunction(function);

                break;
            }

            case Mode::NATIVE_EXECUTOR: {
                if (!function->isNative()) return;

                auto& executor = std::get<NativeExecutor>(mThreadMode);
                executor.function = function;
            }
        }
    }

	void Thread::start() {
        switch (mMode) {
            case Mode::SINGLE_EXECUTOR: {
                auto& executor = std::get<SingleExecutor>(mThreadMode);

                start:

                executor.executor.run();

                if (mMainThread) {
                    setState(State::IDLE);

                    std::unique_lock<std::mutex> lock(mMutex);

                    mCondition.wait(lock, [this] {
                        return mState == State::RUNNABLE; // we can't change the threads execution mode if it's not idle
                    });

                    goto start;
                }

                break;
            }

            case Mode::VTHREAD_EXECUTOR: {
                auto& executor = std::get<VThreadExecutor>(mThreadMode);

                u64 index = 0;

                while (!mInterrupted) {
                    executor.vThreads[index]->executeCycles(5);

                    index = (index + 1) % executor.vThreads.size();
                }

                break;
            }

            case Mode::NATIVE_EXECUTOR: {
                auto& executor = std::get<NativeExecutor>(mThreadMode);

                executor.function->invokeNative<void>();

                break;
            }
        }
	}

	void Thread::stop() {
        std::lock_guard<std::mutex> lock(mMutex);
        mInterrupted = true;
	}
}