#pragma once

#include "pch.h"

typedef struct String {
	char* cString;
	u64 length;
} String;

#define NewConstString(cstr) {(cstr), sizeof(cstr) - 1}
#define NewString(cstr) (String) {(cstr), strlen(cstr)}

#define NullString (String) {null, 0}

#define str(cstr) (String) NewConstString(cstr)