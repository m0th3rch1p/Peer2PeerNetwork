#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UnboundedInteger.h"

struct UnBoundedInteger* unbounded_integer_constructor(short sign, int size, ...) {
	struct UnBoundedInteger* bigNum = (struct UnBoundedInteger*)calloc(1, sizeof(struct UnBoundedInteger));
	if (!bigNum) {
		fprintf(stderr, "[-] error allocating memory for unbounded integer.\n");
		return NULL;
	}

	bigNum->integer = (uint64_t*)calloc(size, sizeof(uint64_t));
	if (!bigNum->integer) {
		fprintf(stderr, "[-] error allocating memory for bignum->integer.\n");
		free(bigNum);
		return NULL;
	}

	bigNum->hex = (char*)calloc(1, size * sizeof(uint64_t));
	if (!bigNum->hex) {
		fprintf(stderr, "[-] error allocating memory for bigNum->hex.\n");
		free(bigNum->integer);
		free(bigNum);
		return NULL;
	}

	bigNum->sign = sign;
	bigNum->size = size;

	va_list* blocks = 0;
#ifdef _WIN32
	__va_start(blocks, size);
#else
	va_start(blocks, size);
#endif
	for (size_t i = 0; i < size; i++)
	{
#ifdef _WIN32
		bigNum->integer[i] = __crt_va_arg(blocks, uint64_t);
#else
		bigNum->integer[i] = va_arg(blocks, uint64_t);
#endif
		char x[17] = { 0 };
		snprintf(x, 16, "%llX", bigNum->integer[i]);
		strcat_s(bigNum->hex, size * sizeof(uint64_t), x);
	}
	
	return bigNum;
}

void unbounded_integer_deconstructor(struct UnBoundedInteger* bigNum) {
	free(bigNum->integer);
	free(bigNum->hex);
	free(bigNum);
}