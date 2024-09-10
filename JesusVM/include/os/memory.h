#pragma once

#include "pch.h"

i32 OSGetPageSize();

void* OSMemoryReserve(u64 size);

void OSMemoryCommit(void* memory, u64 size);

void OSMemoryDecommit(void* memory, u64 size);

void OSMemoryRelease(void* memory, u64 size);