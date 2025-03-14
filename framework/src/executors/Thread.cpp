#include "JesusVM/JesusVM.h"

#include "JesusVM/executors/Thread.h"

#include <algorithm>

namespace JesusVM {
    Thread::Thread()
        : mState(ThreadState::NEW)
        , mInterrupted(false)
        , mFunction(nullptr) {
        setState(ThreadState::IDLE);
    }

    std::thread::id Thread::getId() const {
        return mId;
    }

    ThreadState Thread::getState() const {
        return mState.load(std::memory_order_acquire);
    }

    bool Thread::isDaemon() const {
        return false;
    }

    bool Thread::isInterrupted() const {
        return mInterrupted;
    }

    Executor& Thread::getExecutor() {
        return mExecutor;
    }

    void Thread::setState(ThreadState state) {
        {
            std::lock_guard<std::mutex> lock(mMutex);
            mState.store(state, std::memory_order_release);
        }
        mCondition.notify_all();
    }

    void Thread::setFunction(Function* function) {
        std::unique_lock<std::mutex> lock(mMutex);
        mCondition.wait(lock, [this] {
            return mState == ThreadState::IDLE;
        });

        mFunction = function;
        setState(ThreadState::RUNNABLE);
    }

    void Thread::yield() {
        std::this_thread::yield();
    }

    void Thread::sleep(Long ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    void Thread::start() {
        std::unique_lock<std::mutex> lock(mMutex);
        mCondition.wait(lock, [this] { return (mState == ThreadState::RUNNABLE && mFunction != nullptr) || mInterrupted; });

        if (mInterrupted) return;

        if (mFunction->isNative()) {
            mFunction->invokeNative<void>();
        } else {
            mExecutor.enterFunction(mFunction);
            mExecutor.run();
            setState(ThreadState::IDLE);
        }
    }

    void Thread::interrupt() {
        std::lock_guard<std::mutex> lock(mMutex);
        mInterrupted = true;
        mCondition.notify_all();
    }

    VThreadGroup::VThreadGroup()
        : mState(ThreadState::NEW)
        , mInterrupted(false)
        , mCurrentInterrupted(false)
        , mCurrent(nullptr) {}

    ThreadState VThreadGroup::getState() const {
        return mState.load(std::memory_order_acquire);
    }

    std::thread::id VThreadGroup::getId() const {
        return mId;
    }

    bool VThreadGroup::isDaemon() const {
        return false;
    }

    bool VThreadGroup::isInterrupted() const {
        return mInterrupted;
    }

    u64 VThreadGroup::getThreadCount() const {
        return mThreads.size();
    }

    Executor& VThreadGroup::getExecutor() {
        return mCurrent->getExecutor();
    }

    void VThreadGroup::setState(ThreadState state) {
        {
            std::lock_guard<std::mutex> lock(mMutex);
            mState.store(state, std::memory_order_release);
        }
        mCondition.notify_all();
    }

    void VThreadGroup::runFunction(Function* function) {
        std::lock_guard<std::mutex> lock(mMutex);
        auto thread = std::make_unique<VThread>();
        thread->executeFunction(function);
        mThreads.push_back(std::move(thread));
        mCondition.notify_one();
    }

    void VThreadGroup::yield() {
        std::lock_guard<std::mutex> lock(mMutex);
        if (mCurrent != nullptr) {
            mCurrent->yield();
        }
    }

    void VThreadGroup::sleep(Long ms) {
        std::lock_guard<std::mutex> lock(mMutex);
        if (mCurrent != nullptr) {
            mCurrent->sleep(ms);
        }
    }

    void VThreadGroup::start() {
        using namespace std::chrono_literals;

        while (!mInterrupted) {
            std::unique_lock<std::mutex> lock(mMutex);
            if (mThreads.empty()) {
                if (!mCondition.wait_for(lock, 500ms, [this] { return !mThreads.empty() || mInterrupted; })) {
                    break;
                }
            }

            if (mInterrupted) break;

            for (auto it = mThreads.begin(); it != mThreads.end();) {
                auto& thread = *it;
                if (!thread->isSleeping()) {
                    mCurrent = thread.get();
                    lock.unlock();
                    thread->executeCycles(5);
                    lock.lock();
                }

                if (mCurrentInterrupted) {
                    it = mThreads.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }

    void VThreadGroup::interrupt() {
        std::lock_guard<std::mutex> lock(mMutex);
        mInterrupted = true;
        mCondition.notify_all();
    }

    void VThreadGroup::interruptCurrent() {
        std::lock_guard<std::mutex> lock(mMutex);
        mCurrentInterrupted = true;
        mCondition.notify_all();
    }
}