#include "os/memory.h"

extern SYSTEM_INFO systemInfo;

i32 OSGetPageSize() {
	return systemInfo.dwPageSize;
}

void* OSMemoryReserve(u64 size) {
	return VirtualAlloc(null, size, MEM_RESERVE, PAGE_NOACCESS);
}

void OSMemoryCommit(void* memory, u64 size) {
	VirtualAlloc(memory, size, MEM_COMMIT, PAGE_READWRITE);
}

void OSMemoryDecommit(void* memory, u64 size) {
	VirtualFree(memory, size, MEM_DECOMMIT);
}

void OSMemoryRelease(void* memory, u64 size) {
	VirtualFree(memory, 0, MEM_RELEASE);
}