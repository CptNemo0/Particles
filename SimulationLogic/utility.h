#pragma once
#include "framework.h"

inline static unsigned int hash2uints(unsigned int a, unsigned int b)
{
	return (a * 15823) + (b * 9737333);
}
