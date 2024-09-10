#pragma once

#include "pch.h"
#include "object/heap/rb.h"

typedef struct MemoryRegion MemoryRegion;
typedef struct Heap Heap;

void InitHeap(Heap* heap, u64 regionSize);

void DestroyHeap(Heap* heap);

void* HeapAlloc(Heap* heap, u64 size, u64 alignment);

void HeapFree(Heap* heap, void* memory);

MemoryRegion* HeapFindPointerRegion(Heap* heap, void* memory);

HeapNode* HeapFindNode(Heap* heap, u64 size, u64 alignment, u64* paddingPtr, MemoryRegion** regionPtr);

MemoryRegion* HeapGetMemory(Heap* heap, u64 count);

MemoryRegion* AllocMemoryRegion(u64 size);

void FreeMemoryRegion(MemoryRegion* region);

// gets a pointer to the usable memory of a region
void* GetRegionUsableMemory(MemoryRegion* region);

HeapNode* FindHeapNode(MemoryRegion* region, u64 size, u64 alignment, u64* paddingPtr);

void InsertNode(MemoryRegion* region, HeapNode* newNode);

void RemoveNode(MemoryRegion* region, HeapNode* deleteNode);