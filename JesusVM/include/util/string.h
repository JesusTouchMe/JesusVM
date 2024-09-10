#pragma once

#include "pch.h"

typedef struct String {
	char* buffer;
	u32 length;
	bool isNullTerminated; // if there is a 0 char at buffer[length]
	bool isHeapAllocated; // if the strings buffer is heap allocated
	bool isView; // if the string is a view of an existing string, meaning the buffer doesn't belong to the string
} String;

#define NewConstString(cstr) {(cstr), sizeof(cstr) - 1, true, false, true}

#define NullString (String) {null, 0, true, false, true}

#define str(cstr) (String) NewConstString(cstr)

String NewString(const char* cstr);

String NewStringWithLength(const char* cstr, u32 length);

String NewEmptyString(u32 length);

// will not copy the isView property as it allocates its own buffer
String CopyString(String* other);

String ViewString(String* other);

void DeleteString(String* string);

// Allocated memory from this function will be under control and freed on the next call
char* StringCStr(String* string);

bool StringEquals(String* string, String* other);

i32 StringCompare(String* string, String* other);

u32 IndexOf(String* string, char c);

u32 LastIndexOf(String* string, char c);

String SubStringFrom(String* string, u32 beginIndex);

String SubStringTo(String* string, u32 endIndex);

String SubString(String* string, u32 beginIndex, u32 endIndex);