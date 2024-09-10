#pragma once

#include "object/object.h"
#include "object/heap/rb.h"

Object* AllocObject(Class* clas);

void FreeObject(Object* object);