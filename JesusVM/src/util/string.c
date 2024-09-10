#include "util/string.h"

void NewEmptyString_internal(String* string, u32 length) {
	string->buffer = calloc(length, sizeof(char));

	if (string->buffer == null) {
		puts("string null");
		return;
	}

	string->length = length;
	string->isNullTerminated = false;
	string->isHeapAllocated = true;
	string->isView = false;
}

String NewString(const char* cstr) {
	String string;

	string.buffer = cstr;
	string.length = strlen(cstr);
	string.isNullTerminated = true;
	string.isHeapAllocated = false;
	string.isView = true;

	return string;
}

String NewStringWithLength(const char* cstr, u32 length) {
	String string;

	string.buffer = calloc(length, sizeof(char));

	if (string.buffer == null) {
		puts("string null");
		return NullString;
	}

	string.length = length;

	u64 cstrLength = strlen(cstr);

	string.isNullTerminated = cstrLength < length;
	string.isHeapAllocated = true;
	string.isView = false;

	memcpy_s(string.buffer, length, cstr, cstrLength);

	return string;
}

String NewEmptyString(u32 length) {
	String string;
	NewEmptyString_internal(&string, length);

	return string;
}

String CopyString(String* other) {
	String string;

	if (other->isNullTerminated) {
		string.buffer = calloc(other->length + 1, sizeof(char));
		string.isNullTerminated = true;
	} else {
		string.buffer = calloc(other->length, sizeof(char));
		string.isNullTerminated = true;
	}

	if (string.buffer == null) {
		puts("string null");
		return NullString;
	}

	string.length = other->length;
	string.isHeapAllocated = true;
	string.isView = false;

	memcpy_s(string.buffer, string.length, other->buffer, other->length);

	return string;
}

String ViewString(String* other) {
	String string;

	string.buffer = other->buffer;
	string.length = other->length;
	string.isNullTerminated = other->isNullTerminated;
	string.isHeapAllocated = false;
	string.isView = true;

	return string;
}

void DeleteString(String* string) {
	if (string->isView) { // a view does not own resources and does not need to be deleted
		return;
	}

	if (string->isHeapAllocated) {
		free(string->buffer);
	}

	string->buffer = null;
	string->length = 0;
	string->isHeapAllocated = false;
}

char* StringCStr(String* string) {
	if (string->isNullTerminated) {
		return string->buffer;
	}

	static char* buffer = null;

	if (buffer != null) {
		free(buffer);
	}

	u32 size = string->length + 1;
	buffer = malloc(size);
	
	if (buffer == null) {
		return null;
	}

	memcpy_s(buffer, size, string->buffer, string->length);
	buffer[string->length] = 0;

	return buffer;
}

bool StringEquals(String* string, String* other) {
	u32 length = string->length;

	if (length == other->length) {
		char* buffer1 = string->buffer;
		char* buffer2 = other->buffer;
		
		while (length-- != 0) {
			if (*buffer1++ != *buffer2++) {
				return false;
			}
		}

		return true;
	}

	return false;
}

i32 StringCompare(String* string, String* other) {
	u32 length = string->length < other->length ? string->length : other->length;

	i32 result = memcmp(string->buffer, other->buffer, length);
	if (result == 0) {
		if (string->length < other->length) {
			return -1;
		} else if (string->length > other->length) {
			return 1;
		}
	}

	return result;
}

u32 IndexOf(String* string, char c) {
	u8* buffer = string->buffer;

	for (u32 i = 0; i < string->length; i++) {
		if (buffer[i] == c) {
			return i;
		}
	}

	return -1;
}

u32 LastIndexOf(String* string, char c) {
	u8* buffer = string->buffer;
	
	for (i64 i = string->length - 1; i >= 0; i--) {
		if (buffer[i] == c) {
			return (u32) i;
		}
	}

	return -1;
}

String SubStringFrom(String* string, u32 beginIndex) {
	if (beginIndex == 0) {
		return *string;
	}

	char* substr = string->buffer + beginIndex;
	i64 sublen = string->length - beginIndex;

	if (sublen < 0) {
		puts("string index out of bounds");
		return NullString;
	}

	return (String) { substr, sublen, string->isNullTerminated, false, true };
}

String SubStringTo(String* string, u32 endIndex) {
	if (endIndex == string->length) {
		return *string;
	}
	
	if (endIndex > string->length) {
		puts("string index out of bounds");
		return NullString;
	}

	return (String) { string->buffer, endIndex, false, false, true };
}

String SubString(String* string, u32 beginIndex, u32 endIndex) {
	if (beginIndex == 0 && endIndex == string->length) {
		return *string;
	}

	if (endIndex > string->length) {
		puts("string index out of bounds");
		return NullString;
	}

	char* substr = string->buffer + beginIndex;
	u32 sublen = endIndex - beginIndex;
	bool nullTerminated = endIndex == string->length;

	return (String) { substr, sublen, nullTerminated, false, true };
}