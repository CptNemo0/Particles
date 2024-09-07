#pragma once

#include "framework.h"
#include "BallRepository.h"
#include "glm.hpp"

class DensityEstimator
{
public:

	float target_density = 0.0001f;
	float preassure_factor = 7.5f;

	DensityEstimator() = default;
	DensityEstimator(int a);

	void EstimateDensity(SOARepository& repository, const SpatialHashGrid& grid);
	void EstimatePressure(SOARepository& repository, const SpatialHashGrid& grid);
	float GetDensity(unsigned int id, SOARepository& repository, const SpatialHashGrid& grid);
	glm::vec2 GetPressure(unsigned int id, SOARepository& repository, const SpatialHashGrid& grid);
	float GetInfluence(float radius, float distance);
	float GetInfluenceDerivative(float radius, float distance);

	float DensityToPressure(float density);
};

