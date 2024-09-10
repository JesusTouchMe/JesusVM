#include "type.h"
#include "os/memory.h"
#include "util/io.h"
#include "object/heap/heap.h"
#include "object/heap/heap_functions.h"

typedef struct HeapHeader {
	u64 blockSize;
	u64 padding;
} HeapHeader;

typedef struct MemoryRegion {
	u64 size;
	u64 used;
	HeapNode* root;

	struct MemoryRegion* next;
} MemoryRegion;

typedef struct Heap {
	MemoryRegion* head;
	u64 regionSize;
} Heap;

Heap heap = {0};

static inline u64 GetPaddingWithHeader(u64 ptr, u64 alignment, u64 headerSize) {
	u64 modulo = ptr & (alignment - 1);
	u64 padding = 0;
	u64 neededSpace = 0;

	if (modulo != 0) {
		padding = alignment - modulo;
	}

	neededSpace = headerSize;

	if (padding < neededSpace) {
		neededSpace -= padding;

		if ((neededSpace & (alignment - 1)) != 0) {
			padding += alignment * (1 + neededSpace / alignment);
		} else {
			padding += alignment * (neededSpace / alignment);
		}
	}

	return padding;
}

// initializes a heap with a single region
void InitHeap(Heap* heap, u64 regionSize) {
	heap->regionSize = regionSize;
}

void DestroyHeap(Heap* heap) {
	MemoryRegion* current = heap->head;

	while (current != null) {
		FreeMemoryRegion(current);
	}
}

void* HeapAlloc(Heap* heap, u64 size, u64 alignment) {
	if (size < sizeof(HeapNode)) {
		size = sizeof(HeapNode);
	}

	if (alignment < 8) {
		alignment = 8;
	}

	u64 padding = 0;
	MemoryRegion* region = null;
	HeapNode* node = HeapFindNode(heap, size, alignment, &padding, &region);

	if (node == null) {
		return null;
	}

	u64 alignmentPadding = padding - sizeof(HeapHeader);
	u64 requiredSpace = size + padding;
	u64 remaining = node->blockSize - requiredSpace;

	if (remaining > 0) {
		HeapNode* newNode = (HeapNode*) ((u8*) node + alignmentPadding);
		newNode->blockSize = remaining;
		InsertNode(region, newNode);
	}

	RemoveNode(region, node);

	HeapHeader* header = (HeapHeader*) ((u8*) node + alignmentPadding);
	header->blockSize = requiredSpace;
	header->padding = alignmentPadding;

	region->used += requiredSpace;

	return ((u8*) header + sizeof(HeapHeader));
}

void HeapFree(Heap* heap, void* memory) {
	if (memory == null) {
		return;
	}

	HeapHeader* header = (HeapHeader*) ((u8*) memory - sizeof(HeapHeader));
	HeapNode* freeNode = (HeapNode*) header;

	freeNode->blockSize = header->blockSize + header->padding;

	MemoryRegion* region = HeapFindPointerRegion(heap, memory);
	if (region == null) {
		return;
	}

	HeapNode* node = region->root;

	InsertNode(region, freeNode);

	region->used -= freeNode->blockSize;
}

MemoryRegion* HeapFindPointerRegion(Heap* heap, void* memory) {
	MemoryRegion* current = heap->head;
	
	while (current != null) {
		u8* base = GetRegionUsableMemory(current);

		if (memory >= base && memory <= base + current->size) {
			return current;
		}
	}

	return null;
}

HeapNode* HeapFindNode(Heap* heap, u64 size, u64 alignment, u64* paddingPtr, MemoryRegion** regionPtr) {
	MemoryRegion* current = heap->head;
	HeapNode* node = null;

	while (current != null) {
		node = FindHeapNode(current, size, alignment, paddingPtr);

		if (node != null) {
			if (regionPtr != null) {
				*regionPtr = current;
			}

			return node;
		}

		current = current->next;
	}

	u64 regionCount = (size + heap->regionSize - 1) / heap->regionSize;

	current = HeapGetMemory(heap, regionCount);
	node = FindHeapNode(current, size, alignment, paddingPtr);

	if (regionPtr != null) {
		*regionPtr = current;
	}

	return node;
}

MemoryRegion* HeapGetMemory(Heap* heap, u64 count) {
	u64 finalSize = heap->regionSize * count;

	MemoryRegion* region = AllocMemoryRegion(finalSize);
	if (region == null) {
		return null;
	}

	region->next = heap->head;
	heap->head = region;

	return region;
}

MemoryRegion* AllocMemoryRegion(u64 size) {
	MemoryRegion* region = OSMemoryReserve(size);
	if (region == null) {
		return null;
	}

	OSMemoryCommit(region, size);

	region->size = size;
	region->used = sizeof(MemoryRegion); // the region is allocated within itself
	region->root = null;
	region->next = null;

	RBInsert(&region->root, (HeapNode*) GetRegionUsableMemory(region));
}

void FreeMemoryRegion(MemoryRegion* region) {
	OSMemoryRelease(region, region->size);
}

void* GetRegionUsableMemory(MemoryRegion* region) {
	return region + 1;
}

HeapNode* FindHeapNode(MemoryRegion* region, u64 size, u64 alignment, u64* paddingPtr) {
	HeapNode* current = region->root;
	HeapNode* best = null;
	
	u64 padding = 0;

	while (current != null) {
		padding = GetPaddingWithHeader((u64) current, alignment, sizeof(HeapNode));
		u64 requiredSpace = size + padding;

		// if it matches perfecdtly there's no need to keep looking
		if (current->blockSize == requiredSpace) {
			best = current;
			break;
		} else if (current->blockSize >= requiredSpace) {
			best = current;
			current = current->left;
		} else {
			current = current->right;
		}
	}

	if (paddingPtr != null) *paddingPtr = padding;

	return best;
}

void InsertNode(MemoryRegion* region, HeapNode* newNode) {
	RBInsert(&region->root, newNode);
}

void RemoveNode(MemoryRegion* region, HeapNode* deleteNode) {
	RBRemove(&region->root, deleteNode);
}

Object* AllocObject(Class* class) { // TODO: make good heap
	if (heap.head == null) { // uninitialized heap
		InitHeap(&heap, OSGetPageSize());
	}

	/*
	Printf(str("Allocating object of type %s\n"), class->name);

	Object* obj = malloc(sizeof(Object) + sizeof(Field) * class->fieldRefCount);
	if (obj == null) {
		puts("allocation failed");
		exit(1);
	}

	obj->methods = malloc(sizeof(Method) * class->methodRefCount);
	if (obj->methods == null) {
		free(obj);
		puts("method allocation failed");
		exit(1);
	}

	for (u16 i = 0; i < class->fieldRefCount; i++) {
		obj->fields[i].type = class->fieldRefs[i].type;
		obj->fields[i].value.i = 0;
	}

	for (u16 i = 0; i < class->methodRefCount; i++) {
		obj->methods[i].function = class->methodRefs[i].function;
	}

	obj->type = class;
	obj->refCount = 1; // give a reference to the caller

	return obj;
	*/
}

void FreeObject(Object* object) { // TODO: make good heap
	/*
	Printf(str("Freeing object of type %s\n"), object->type->name);
	
	free(object->methods);

	for (u16 i = 0; i < object->type->fieldRefCount; i++) {
		if (!object->fields[i].type->isPrimitive) { // it has an object
			RemoveReference(object->fields[i].value.ref);
		}
	}

	free(object);
	*/
}