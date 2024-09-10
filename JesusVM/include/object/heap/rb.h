// handles the red black tree for the free nodes

#pragma once

#include "pch.h"

#define RED 0
#define BLACK 1

typedef struct HeapNode {
	struct HeapNode* left;
	struct HeapNode* right;
	struct HeapNode* parent;

	u8 color;

	u64 blockSize;
} HeapNode;

void RBLeftRotate(HeapNode** root, HeapNode* currentNode);

void RBRightRotate(HeapNode** root, HeapNode* currentNode);

void RBTransplant(HeapNode** root, HeapNode* oldNode, HeapNode* newNode);

HeapNode* RBMinimum(HeapNode* node);

void RBInsertFixUp(HeapNode** root, HeapNode* newNode);

void RBInsert(HeapNode** root, HeapNode* newNode);

void RBRemoveFixUp(HeapNode** root, HeapNode* fixNode);

void RBRemove(HeapNode** root, HeapNode* deleteNode);