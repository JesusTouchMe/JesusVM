// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_UTIL_PROFILER_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_UTIL_PROFILER_H

#include "JesusVM/util/Logging.h"

#include "JesusVM/Options.h"

#include <cassert>
#include <chrono>

namespace JesusVM::Profiler {
    using namespace std::chrono;

    template<size_t PhaseCount, class TimeUnit = microseconds, class Clock = high_resolution_clock>
    requires is_clock_v<Clock>
    class GCProfiler {
    public:
        using Duration = duration<double, typename TimeUnit::period>;

        struct Phase {
            std::string_view name;
            Duration totalTime{0};
            u64 callCount{0};
            Clock::time_point startTime;
        };

        constexpr GCProfiler(std::initializer_list<const char*> phaseNames) {
            assert(phaseNames.size() == PhaseCount && "Phase count mismatch with template argument PhaseCount");

            u64 i = 0;
            for (auto name : phaseNames) {
                mPhases[i++].name = name;
            }
        }

        void start() {
            if (!opt::gcProfilerEnabled) return;

            mGcStartTime = Clock::now();
        }

        void startPhase(u64 phase) {
            if (!opt::gcProfilerEnabled) return;

            assert(phase < PhaseCount);
            mPhases[phase].startTime = Clock::now();
        }

        void endPhase(u64 phase) {
            if (!opt::gcProfilerEnabled) return;

            assert(phase < PhaseCount);

            auto now = Clock::now();
            mPhases[phase].totalTime += now - mPhases[phase].startTime;
            mPhases[phase].callCount++;
        }

        void end() {
            if (!opt::gcProfilerEnabled) return;

            auto now = Clock::now();
            mTotalCycleTime += now - mGcStartTime;
            mTotalCycleCount++;
        }

        void reset() {
            if (!opt::gcProfilerEnabled) return;

            for (auto& phase : mPhases) {
                phase.totalTime = Duration{0};
                phase.callCount = 0;
            }

            mTotalCycleTime = Duration{0};
            mTotalCycleCount = 0;
        }

        void logStats() const {
            if (!opt::gcProfilerEnabled) return;

            Logger::Log("=== GC Profile ===");
            Logger::Format("Total GC cycles: {}, total time: {:.3f} {}", mTotalCycleCount, mTotalCycleTime.count(), unitSuffix());
            for (const auto& phase : mPhases) {
                Logger::Format("Phase {:<20} Total time: {:>10.3f} {}, Calls: {:>5}",
                               phase.name, phase.totalTime.count(), unitSuffix(), phase.callCount);
            }
            Logger::Log("==================");
        }

        std::string generateGraphviz(std::string_view gcName = "GC") const {
            if (!opt::gcProfilerEnabled) return "";

            std::string result;
            result += std::format("digraph \"{}_Profile\" {{\n", gcName);
            result += "    rankdir=LR;\n";
            result += "    node [shape=record];\n";
            result += std::format("    GC [label=\"{{GC Cycle|Total Cycles: {}|Total Time: {} {}}}\", style=bold];\n",
                                  mTotalCycleCount, mTotalCycleTime.count(), unitSuffix());

            for (size_t i = 0; i < mPhases.size(); i++) {
                result += std::format("    phase_{} [label=\"{{{}|Total Time: {}{}|Calls: {}}}\"];\n",
                                      i, mPhases[i].name, mPhases[i].totalTime.count(), unitSuffix(), mPhases[i].callCount);
                if (i == 0)
                    result += std::format("    GC -> phase_{};\n", i);
                else
                    result += std::format("    phase_{} -> phase_{};\n", i - 1, i);
            }

            result += "}\n";
            return result;
        }

    private:
        std::array<Phase, PhaseCount> mPhases;
        Clock::time_point mGcStartTime;
        u64 mTotalCycleCount{0};
        Duration mTotalCycleTime{0};

        constexpr const char* unitSuffix() const {
            if constexpr (std::is_same_v<TimeUnit, std::chrono::microseconds>)
                return "µs";
            else if constexpr (std::is_same_v<TimeUnit, std::chrono::milliseconds>)
                return "ms";
            else if constexpr (std::is_same_v<TimeUnit, std::chrono::seconds>)
                return "s";
            else
                return "units";
        }
    };
}

#endif //JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_UTIL_PROFILER_H
