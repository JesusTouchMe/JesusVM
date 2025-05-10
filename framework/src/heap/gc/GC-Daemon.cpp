// Copyright 2025 JesusTouchMe

#include "JesusVM/executors/Threading.h"

#include "JesusVM/heap/gc/CycleCollector.h"
#include "JesusVM/heap/gc/GC-Daemon.h"

namespace JesusVM::GC::Daemon {
    class Daemon* gcDaemon = nullptr;

    volatile bool finalize = false;

    bool isReady = false;
    std::mutex readyMutex;
    std::condition_variable readyCondition;

    static void Entry(DaemonContext ctx) {
        while (true) {
            ctx.hibernate();

            std::lock_guard<std::mutex> lock(readyMutex);

            isReady = false;
            ProcessCycles();
            isReady = true;

            readyCondition.notify_one();

            if (finalize) break;
        }
    }

    void Launch() {
        gcDaemon = Threading::LaunchDaemon("GC-Daemon", Entry, true);
        isReady = true;
    }

    void Finalize() {
        finalize = true;
    }

    bool IsReady() {
        return isReady;
    }

    void WaitUntilReady() {
        std::unique_lock<std::mutex> lock(readyMutex);
        readyCondition.wait(lock, []() {
            return IsReady();
        });
    }

    void BeginCollection() {
        std::lock_guard<std::mutex> lock(readyMutex);
        gcDaemon->wake();
    }
}