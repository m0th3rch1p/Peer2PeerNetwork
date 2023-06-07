#pragma once

#ifndef UNBOUNDED_INTEGER_H
#define UNBOUNDED_INTEGER_H
#include <stdint.h>

struct UnBoundedInteger {
	uint64_t* integer;
	short sign;
	int size;
	char* hex;
};

struct UnBoundedInteger* unbounded_integer_constructor(short sign, int size, ...);
void unbounded_integer_deconstructor(struct UnBoundedInteger* bigNum);
#endif