// Copyright 2025 JesusTouchMe

#include "JesusVM/executors/Daemon.h"

#include "JesusVM/heap/gc/CycleCollector.h"
#include "JesusVM/heap/gc/GC-Daemon.h"

namespace JesusVM::GC::Daemon {
    class Daemon* gcDaemon = nullptr;

    [[noreturn]] static void Entry(DaemonContext ctx) {
        while (true) {
            ctx.hibernate();
            ProcessCycles();
        }
    }

    void Launch() {
        gcDaemon = Threading::LaunchDaemon("GC-Daemon", Entry);
    }

    void BeginCollection() {
        if (gcDaemon->isSleeping()) {
            gcDaemon->wake();
        }
    }
}