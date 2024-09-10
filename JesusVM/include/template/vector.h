#pragma once

#include "pch.h"

#define DeclareVector(T) \
typedef struct _Vector##T##Functions { \
	void (*Delete)(struct _Vector##T*); \
	void (*Add)(struct _Vector##T*, T); \
	void (*AddFront)(struct _Vector##T*, T); \
	void (*Insert)(struct _Vector##T*, T, u64); \
} _Vector##T##Functions; \
\
typedef struct _Vector##T { \
	T* data; \
	u64 size; \
	u64 capacity; \
	struct _Vector##T##Functions _functions; \
} _Vector##T; \
\
_Vector##T _tNewVector##T()

#define DefineVector(T) \
struct _Vector##T; \
\
void _tVectorCheckCapacity##T(_Vector##T* vector) { \
	if (vector->size >= vector->capacity) { \
		vector->capacity = (vector->capacity == 0) ? 2 : vector->capacity << 1; \
		vector->data = realloc(vector->data, sizeof(T) * vector->capacity); \
	} \
} \
\
void _tDeleteVector##T(_Vector##T* vector) { \
	if (vector->data != null) { \
		free(vector->data); \
	} \
} \
\
void _tVectorAdd##T(_Vector##T* vector, T value) { \
	_tVectorCheckCapacity##T(vector); \
\
	vector->data[vector->size++] = value; \
} \
\
void _tVectorAddFront##T(_Vector##T* vector, T value) { \
	for (u64 i = vector->size; i > 0;) { \
		vector->data[i] = vector->data[--i]; \
	} \
\
	vector->data[0] = value; \
	vector->size++; \
} \
\
void _tVectorInsert##T(_Vector##T* vector, T value, u64 index) { \
	if (index > vector->size) { \
		puts("Vector<" #T "> index out of bounds"); \
		return; \
	} \
\
	for (u64 i = vector->size; i > index;) { \
		vector->data[i] = vector->data[--i]; \
	} \
\
	vector->data[index] = value; \
	vector->size++; \
} \
\
static _Vector##T##Functions _tVectorFunctions##T = { \
	.Delete = _tDeleteVector##T, \
	.Add = _tVectorAdd##T, \
	.AddFront = _tVectorAddFront##T, \
	.Insert = _tVectorInsert##T, \
}; \
\
_Vector##T _tNewVector##T() { \
	_Vector##T vector = {0}; \
	vector._functions = _tVectorFunctions##T; \
	return vector; \
}\

#define Vector(T) _Vector##T
#define NewVector(T) _tNewVector##T()
#define MoveVector(vector) (Vector) { (vector)->data, (vector)->size, (vector)->capacity, (vector)->_functions }
#define DeleteVector(vector) (vector)->_functions.Delete(vector)

#define VectorBack(vector) ((vector)->data[(vector)->size - 1])

#define VectorAdd(vector, value) (vector)->_functions.Add(vector, value)

#define VectorFront(vector, value) (vector)->_functions.AddFront(vector, value)

#define VectorInsert(vector, value, index) (vector)->_functions.Insert(vector, value, index)