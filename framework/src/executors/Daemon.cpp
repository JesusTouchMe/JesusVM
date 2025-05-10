// Copyright 2025 JesusTouchMe

#include "JesusVM/executors/Daemon.h"
#include "JesusVM/executors/Threading.h"

#include <utility>

namespace JesusVM {
    void DaemonContext::hibernate() {
        std::unique_lock<std::mutex> lock(thread->mMutex);

        thread->mSleeping = true;

        thread->mWakeCondition.wait(lock, [this]() {
            return !thread->mSleeping;
        });
    }

    void DaemonContext::sleep(Long ms) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

    void DaemonContext::yield() {
        std::this_thread::yield();
    }

    Daemon::Daemon(std::string_view name, std::function<void(DaemonContext)> entry)
        : mName(name)
        , mEntry(std::move(entry))
        , mSleeping(true) {}

    std::string_view Daemon::getName() const {
        return mName;
    }

    std::thread::id Daemon::getId() const {
        return mId;
    }

    bool Daemon::isSleeping() const {
        return mSleeping;
    }

    void Daemon::wake() {
        mSleeping = false;
        mWakeCondition.notify_all();
    }

    void Daemon::start() {
        DaemonContext ctx { this };
        ctx.hibernate();
        mEntry(ctx);
    }
}