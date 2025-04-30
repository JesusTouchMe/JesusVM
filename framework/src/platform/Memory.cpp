// Copyright 2025 JesusTouchMe

#include "JesusVM/platform/Memory.h"

#if defined(PLATFORM_WINDOWS)

#include <windows.h>
#include <psapi.h>

#elif defined(PLATFORM_LINUX)

#include <fstream>
#include <string>

#elif defined(PLATFORM_MACOS)

#include <mach/mach.h>

#endif

namespace Platform {
    std::size_t GetUsedMemory() {
#if defined(PLATFORM_WINDOWS)
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
            return static_cast<std::size_t>(pmc.WorkingSetSize);
        }

        return 0;

#elif defined(PLATFORM_LINUX)
        std::ifstream statusFile("/proc/self/status");
        std::string line;

        while (std::getline(statusFile, line)) {
            if (line.rfind("VmRSS:", 0) == 0) {
                std::size_t kb = 0;
                std::sscanf(line.c_str(), "VmRSS: %zu kB", &kb);
                return kb * 1024; // bytes
            }
        }

        return 0;

#elif defined(PLATFORM_MACOS)
        mach_task_basic_info info;
        mach_msg_type_number_t infoCount = MACH_TASK_BASIC_INFO_COUNT;

        if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, reinterpret_cast<task_info_t>(&info), &infoCount) == KERN_SUCCESS) {
            return static_cast<std::size_t>(info.resident_size); // in bytes
        }

        return 0;

#endif
    }
}
