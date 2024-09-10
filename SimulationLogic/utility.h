#pragma once
#include "framework.h"

static inline unsigned int hash2uints(unsigned int a, unsigned int b)
{
	return (a * 15823) + (b * 9737333);
}

static inline unsigned int hash3uints(unsigned int a, unsigned int b, unsigned int c)
{
	return (a * 15823) + (b * 9737333) + (c * 131071);
}
