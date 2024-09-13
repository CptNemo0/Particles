#pragma once
#include "framework.h"

static inline unsigned int hash2uints(unsigned int a, unsigned int b)
{
	return (a * 15823) + (b * 9737333);
}

static inline unsigned int hash3uints(unsigned int a, unsigned int b, unsigned int c)
{
	return (a * 15823) + (b * 9737333) + (c * 1279457);
}

static inline float poly6(float distance)
{
	constexpr float h = INFLUENCE_RADIUS;
	constexpr float hpow9 = h * h * h * h * h * h * h * h * h;
	constexpr float hpow2 = h * h;
	constexpr float factor = 315.0f / (64.0f * 3.14159265359f * hpow9);

	float a = 0.0f;

	if (0 <= distance && distance <= h)
	{
		a = (hpow2 - (distance * distance));
		a = a * a * a;
	}

	return factor * a;	
}

static inline glm::vec3 spiky_gradient(float x, float y, float z)
{
	constexpr float h = INFLUENCE_RADIUS;
	constexpr float hpow6 = h * h * h * h * h * h;
	constexpr float factor = (45.0f / (3.14159265359f * hpow6));

	glm::vec3 return_value = glm::vec3(0.0f);
	glm::vec3 r(x, y, z);
	
	float magnitude = glm::length(r) + 0.0000001f;
	
	if (0 < magnitude && magnitude < h)
	{
		return_value = (h - magnitude) * (h - magnitude) * factor * r / magnitude;
	}

	return return_value;
}

static inline float spiky(float distance)
{
	constexpr float h = INFLUENCE_RADIUS;
	constexpr float hpow6 = h * h * h * h * h * h;
	constexpr float factor = 45.0f / (3.14159265359f * hpow6);

	float a = 0.0f;

	if (0 <= distance && distance <= h)
	{
		a = (h - distance);
		a *= a;
	}

	return a * factor;
}