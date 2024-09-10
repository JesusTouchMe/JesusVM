#include "util/io.h"
#include "util/io.h"
#include "stdarg.h"

static inline void WriteOut(String str) {
	fwrite(str.buffer, sizeof(char), str.length, stdout);
}

static inline void WriteOutP(String* str) {
	fwrite(str->buffer, sizeof(char), str->length, stdout);
}

static u32 IntToString(char* buffer, i32 value) {
	if (value == 0) {
		buffer[0] = '0';
		return 1;
	}

	u32 i = 0;
	bool is_negative = false;

	if (value < 0) {
		is_negative = true;
		value = -value;
	}

	while (value != 0) {
		buffer[i++] = '0' + (value % 10);
		value /= 10;
	}

	if (is_negative) {
		buffer[i++] = '-';
	}

	// Reverse the string
	for (i32 j = 0; j < i / 2; ++j) {
		char temp = buffer[j];
		buffer[j] = buffer[i - j - 1];
		buffer[i - j - 1] = temp;
	}

	return i;
}

void Print(String str) {
	WriteOut(str);
}

void PrintLn(String str) {
	WriteOut(str);
	WriteOut(str("\n"));
}

void Printf(String format, ...) {
	va_list args;
	va_start(args, format);

	const char* fmt = format.buffer;
	const u64 length = format.length;

	for (u64 i = 0; i < length; i++) {
		if (fmt[i] == '%') {
			i++;

			if (i < length) {
				switch (fmt[i]) {
					case 'c': {
						char c = (char) va_arg(args, char);
						putchar(c);
						break;
					}

					case 's': {
						String* sp;

						if (i < length && fmt[i + 1] == 'p') {
							i++;
							sp = (String*) va_arg(args, String*);
						} else {
							String s = va_arg(args, String);
							sp = &s;
						}

						if (sp->buffer == null) {
							WriteOut(str("null"));
						} else {
							WriteOutP(sp);
						}

						break;
					}

					case 'd': case 'i': {
						i32 d = va_arg(args, i32);
						
						char intStr[32];
						u32 len = IntToString(intStr, d);

						WriteOut((String) { intStr, len });
					}
				}
			} else {
				putchar('%');
			}
		} else {
			putchar(fmt[i]);
		}
	}
}