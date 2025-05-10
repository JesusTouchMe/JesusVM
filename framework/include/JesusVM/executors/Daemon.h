// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_EXECUTORS_DAEMON_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_EXECUTORS_DAEMON_H

#include "JesusVM/JesusNative.h"

#include <condition_variable>
#include <functional>
#include <string_view>
#include <thread>

// Class for internal daemons for the VM, not for language-level daemon threads which get managed by standard Thread
// Can sleep and wake up

namespace JesusVM {
    class Daemon;
    class MutationBuffer;

    struct DaemonContext {
        Daemon* thread;

        void hibernate();
        void sleep(Long ms);
        void yield();
    };

    namespace Threading {
        Daemon* LaunchDaemon(std::string_view name, std::function<void(DaemonContext)> entry, bool crucial);
    }

    // Daemon::start starts out asleep and will be automatically woken up by LaunchDaemon
    class Daemon {
    friend struct DaemonContext;
    friend Daemon* Threading::LaunchDaemon(std::string_view name, std::function<void(DaemonContext)> entry, bool crucial);
    public:
        Daemon(std::string_view name, std::function<void(DaemonContext)> entry);

        std::string_view getName() const;
        std::thread::id getId() const;
        bool isSleeping() const;

        void wake();

        void start();

    private:
        std::string_view mName;
        std::thread::id mId;
        std::function<void(DaemonContext)> mEntry;

        std::mutex mMutex;
        std::condition_variable mWakeCondition;

        bool mSleeping;
    };
}

#endif //JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_EXECUTORS_DAEMON_H
