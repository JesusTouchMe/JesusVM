#include "object/heap/rb.h"

void RBLeftRotate(HeapNode** root, HeapNode* currentNode) {
    if (currentNode == null || currentNode->right == null) {
        return;
    }

    HeapNode* rightChild = currentNode->right;

    currentNode->right = rightChild->left;

    if (currentNode->right != null) {
        currentNode->right->parent = currentNode;
    }

    rightChild->parent = currentNode->parent;

    if (currentNode->parent == null) {
        *root = rightChild;
    } else if (currentNode == currentNode->parent->left) {
        currentNode->parent->left = rightChild;
    } else {
        currentNode->parent->right = rightChild;
    }

    rightChild->left = currentNode;
    currentNode->parent = rightChild;
}

void RBRightRotate(HeapNode** root, HeapNode* currentNode) {
	if (currentNode == null || currentNode->left == null) {
		return;
	}

	HeapNode* leftChild = currentNode->left;

	currentNode->left = leftChild->right;

	if (leftChild->right != null) {
		leftChild->right->parent = currentNode;
	}

	leftChild->parent = currentNode->parent;

	if (currentNode->parent == null) {
		*root = leftChild;
	} else if (currentNode == currentNode->parent->left) {
		currentNode->parent->left = leftChild;
	} else {
		currentNode->parent->right = leftChild;
	}

	leftChild->right = currentNode;
	currentNode->parent = leftChild;
}

void RBTransplant(HeapNode** root, HeapNode* oldNode, HeapNode* newNode) {
    if (oldNode->parent == null) {
        *root = newNode;
    } else if (oldNode == oldNode->parent->left) {
        oldNode->parent->left = newNode;
    } else {
        oldNode->parent->right = newNode;
    }

    if (newNode != null) {
        newNode->parent = oldNode->parent;
    }
}

HeapNode* RBMinimum(HeapNode* node) {
    while (node->left != null) {
        node = node->left;
    }

    return node;
}

void RBInsertFixUp(HeapNode** root, HeapNode* newNode) {
    while (newNode != *root && newNode->parent->color == RED) {
        HeapNode* uncleNode;

        if (newNode->parent == newNode->parent->parent->left) {
            uncleNode = newNode->parent->parent->right;
        } else {
            uncleNode = newNode->parent->parent->left;
        }

        if (uncleNode == null) {
            newNode = newNode->parent->parent;
        } else if (uncleNode->color == RED) {
            uncleNode->color = BLACK;
            newNode->parent->color = BLACK;
            newNode->parent->parent->color = RED;
            newNode = newNode->parent->parent;
        } else {
            if (newNode->parent == newNode->parent->parent->left && newNode == newNode->parent->left) {
                u8 color= newNode->parent->color;
                newNode->parent->color = newNode->parent->parent->color;
                newNode->parent->parent->color = color;

                RBRightRotate(root, newNode->parent->parent);
            }

            if (newNode->parent == newNode->parent->parent->left && newNode == newNode->parent->right) {
                u8 color = newNode->color;
                newNode->color = newNode->parent->parent->color;
                newNode->parent->parent->color = color;

                RBLeftRotate(root, newNode->parent);
                RBRightRotate(root, newNode->parent->parent);
            }

            if (newNode->parent == newNode->parent->parent->right && newNode == newNode->parent->right) {
                u8 color = newNode->parent->color;
                newNode->parent->color = newNode->parent->parent->color;
                newNode->parent->parent->color = color;

                RBLeftRotate(root, newNode->parent->parent);
            }

            if (newNode->parent == newNode->parent->parent->right && newNode == newNode->parent->left) {
                u8 color = newNode->color;
                newNode->color = newNode->parent->parent->color;
                newNode->parent->parent->color = color;

                RBRightRotate(root, newNode->parent);
                RBLeftRotate(root, newNode->parent->parent);
            }
        }
    }

    (*root)->color = BLACK;
}

void RBInsert(HeapNode** root, HeapNode* newNode) {
    newNode->left = null;
    newNode->right = null;
    newNode->parent = null;

    if (*root == null) {
        newNode->color = BLACK;
        *root = newNode;
    } else {
        HeapNode* currentNode = *root;
        HeapNode* parentNode = null;

        while (currentNode != null) {
            parentNode = currentNode;

            if (newNode->blockSize < currentNode->blockSize) {
                currentNode = currentNode->left;
            } else {
                currentNode = currentNode->right;
            }
        }

        newNode->parent = parentNode;

        if (newNode->blockSize > parentNode->blockSize) {
            parentNode->right = newNode;
        } else {
            parentNode->left = newNode;
        }

        newNode->color = RED;
        RBInsertFixUp(root, newNode);
    }
}

void RBRemoveFixUp(HeapNode** root, HeapNode* fixNode) {
    while (fixNode != *root && fixNode->color == BLACK) {
        if (fixNode == fixNode->parent->left) {
            HeapNode* siblingNode = fixNode->parent->right;

            if (siblingNode->color == RED) {
                siblingNode->color = BLACK;
                fixNode->color = RED;
                RBLeftRotate(root, fixNode->parent);
                siblingNode = fixNode->parent->right;
            }

            if (siblingNode->left->color == BLACK && siblingNode->right->color == BLACK) {
                siblingNode->color = RED;
                fixNode = fixNode->parent;
            } else {
                if (siblingNode->right->color == BLACK) {
                    siblingNode->left->color = BLACK;
                    siblingNode->color = RED;
                    RBRightRotate(root, siblingNode);
                    siblingNode = fixNode->parent->right;
                }

                siblingNode->color = fixNode->parent->color;
                fixNode->parent->color = BLACK;
                siblingNode->right->color = BLACK;
                RBLeftRotate(root, fixNode->parent);
                fixNode = *root;
            }
        } else {
            HeapNode* siblingNode = fixNode->parent->left;

            if (siblingNode->color == RED) {
                siblingNode->color = BLACK;
                fixNode->parent->color = RED;
                RBRightRotate(root, fixNode->parent);
                siblingNode = fixNode->parent->left;
            }

            if (siblingNode->right->color == BLACK && siblingNode->left->color == BLACK) {
                siblingNode->color = RED;
                fixNode = fixNode->parent;
            } else {
                if (siblingNode->left->parent == BLACK) {
                    siblingNode->right->color = BLACK;
                    siblingNode->color = RED;
                    RBLeftRotate(root, siblingNode);
                    siblingNode = fixNode->parent->left;
                }

                siblingNode->color = fixNode->parent->color;
                fixNode->parent->color = BLACK;
                siblingNode->left->color = BLACK;
                RBRightRotate(root, fixNode->parent);
                fixNode = *root;
            }
        }
    }

    fixNode->color = BLACK;
}

void RBRemove(HeapNode** root, HeapNode* deleteNode) {
    HeapNode* replacement = deleteNode;
    HeapNode* fixNode;
    u8 color = replacement->color;

    if (deleteNode->left == null) {
        fixNode = deleteNode->right;
        RBTransplant(root, deleteNode, deleteNode->right);
    } else if (deleteNode->right == null) {
        fixNode = deleteNode->left;
        RBTransplant(root, deleteNode, deleteNode->left);
    } else {
        replacement = RBMinimum(deleteNode->right);
        color = replacement->color;
        fixNode = replacement->right;

        if (replacement->parent = deleteNode) {
            if (fixNode != null) {
                fixNode->parent = replacement;
            }
        } else {
            RBTransplant(root, replacement, replacement->right);
            replacement->right = deleteNode->right;
            replacement->right->parent = replacement;
        }

        RBTransplant(root, deleteNode, replacement);
        replacement->left = deleteNode->left;
        replacement->left->parent = replacement;
        replacement->color = deleteNode->color;
    }

    if (color == BLACK) {
        RBRemoveFixUp(root, fixNode);
    }
}