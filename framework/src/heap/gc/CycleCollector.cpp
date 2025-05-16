// Copyright 2025 JesusTouchMe

#include "JesusVM/heap/gc/CycleCollector.h"

#include "JesusVM/heap/Object.h"

#include "JesusVM/util/Profiler.h"

#include <algorithm>
#include <fstream>
#include <vector>

namespace JesusVM::GC {
    std::vector<Object*> cycleRoots;

    std::mutex mutex;

    Profiler::GCProfiler<5> profiler {
        "Free Cycles",
        "Mark Roots",
        "Scan Roots",
        "Collect Roots",
        "Sigma Preparation"
    };

    struct Cycle {
        std::vector<Object*> cycle;

        bool deltaTest() {
            return std::ranges::all_of(cycle, [](Object* object) {
                return object->getColor() == Object::ORANGE;
            });
        }

        bool sigmaTest() {
            i32 externRefCount = 0;
            for (Object* object : cycle) {
                externRefCount += object->getCyclicReferenceCount();
            }

            return (externRefCount == 0);
        }

        void freeCycle() {
            for (Object* object : cycle) {
                object->mColor = Object::RED;
            }

            for (Object* object : cycle) {
                object->finalize();
            }

            for (Object* object : cycle) {
                object->forEachChild([](Object* child) {
                    child->cyclicDecrement();
                });
            }

            for (Object* object : cycle) {
                object->free();
            }
        }

        void refurbish() {
            if (cycle.empty()) return;

            auto handleObject = [](Object* object, bool includeOrange) {
                if ((includeOrange && object->mColor == Object::ORANGE) || object->mColor == Object::PURPLE) {
                    object->mColor = Object::PURPLE;
                    cycleRoots.push_back(object);
                } else {
                    object->mColor = Object::BLACK;
                    object->mBuffered = false;
                }
            };

            handleObject(cycle.front(), true);
            for (auto it = cycle.begin() + 1; it != cycle.end(); ++it) {
                handleObject(*it, false);
            }
        }
    };

    std::vector<Cycle> cycleBuffer;

    void AddPossibleCycleRoot(Object* object) {
        object->scanBlack();
        object->mColor = Object::PURPLE;
        if (!object->mBuffered) {
            object->mBuffered = true;
            cycleRoots.push_back(object);
        }
    }

    void MarkRoots() {
        profiler.startPhase(1);

        for (auto it = cycleRoots.begin(); it != cycleRoots.end();) {
            Object* object = *it;

            if (object->mColor == Object::GRAY || (object->mColor == Object::PURPLE && object->getReferenceCount() > 0)) {
                object->markGray();
                ++it;
            } else {
                it = cycleRoots.erase(it);
                object->mBuffered = false;

                if (object->getReferenceCount() <= 0) {
                    object->free();
                }
            }
        }

        profiler.endPhase(1);
    }

    void ScanRoots() {
        profiler.startPhase(2);

        for (Object* object : cycleRoots) {
            object->scanRoot();
        }

        profiler.endPhase(2);
    }

    void CollectRoots() {
        profiler.startPhase(3);

        for (auto it = cycleRoots.begin(); it != cycleRoots.end();) {
            Object* object = *it;

            if (object->mColor == Object::WHITE) {
                std::vector<Object*> currentCycle;
                object->collectWhite(currentCycle);
                cycleBuffer.emplace_back(std::move(currentCycle));
            } else {
                object->mBuffered = false;
            }

            it = cycleRoots.erase(it);
        }

        profiler.endPhase(3);
    }

    void FreeCycles() {
        profiler.startPhase(0);

        i64 last = cycleBuffer.size() - 1;
        for (i64 i = last; i >= 0; i--) {
            auto& cycle = cycleBuffer[i];
            if (cycle.deltaTest() && cycle.sigmaTest()) {
                cycle.freeCycle();
            } else {
                cycle.refurbish();
            }
        }

        cycleBuffer.clear();

        profiler.endPhase(0);
    }

    void CollectCycles() {
        MarkRoots();
        ScanRoots();
        CollectRoots();
    }

    void SigmaPreparation() {
        profiler.startPhase(4);

        for (auto& cycle : cycleBuffer) {
            for (Object* object : cycle.cycle) {
                object->mColor = Object::RED;
                object->mCyclicRefCount = object->getReferenceCount();
            }

            for (Object* object : cycle.cycle) {
                object->forEachChild([](Object* child) {
                    if (child->mColor == Object::RED && child->mCyclicRefCount > 0) {
                        child->mCyclicRefCount -= 1;
                    }
                });
            }

            for (Object* object : cycle.cycle) {
                object->mColor = Object::ORANGE;
            }
        }

        profiler.endPhase(4);
    }

    void ProcessCycles() {
        std::lock_guard<std::mutex> lock(mutex);

        profiler.reset();
        profiler.start();

        FreeCycles();
        CollectCycles();
        SigmaPreparation();

        profiler.end();

        profiler.logStats();
    }
}