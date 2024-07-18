#include "util.h"
#include "pch.h"

void PrintBinary(i32 bits, u64 num) {
	for (i32 i = bits - 1; i >= 0; i--) {
		if (num & ((u64) 1 << i))
			printf("1");
		else
			printf("0");
	}

	printf("\n");
}