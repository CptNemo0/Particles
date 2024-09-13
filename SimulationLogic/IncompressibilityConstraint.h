#pragma once

#include <map>
#include "BallRepository.h"
#include "SpatialHashGrid.h"
#include "utility.h"
#include "glm/glm.hpp"

class IncompressibilityConstraint
{
private:
	std::vector<std::tuple<int, int, int>> offsets_3d
	{
		{-1, -1, -1},
		{-1, -1,  0},
		{-1, -1,  1},

		{-1,  0, -1},
		{-1,  0,  0},
		{-1,  0,  1},

		{-1,  1, -1},
		{-1,  1,  0},
		{-1,  1,  1},

		{ 0, -1, -1},
		{ 0, -1,  0},
		{ 0, -1,  1},

		{ 0,  0, -1},
		{ 0,  0,  0},
		{ 0,  0,  1},

		{ 0,  1, -1},
		{ 0,  1,  0},
		{ 0,  1,  1},

		{ 1, -1, -1},
		{ 1, -1,  0},
		{ 1, -1,  1},

		{ 1,  0, -1},
		{ 1,  0,  0},
		{ 1,  0,  1},

		{ 1,  1, -1},
		{ 1,  1,  0},
		{ 1,  1,  1},
	};
public:
	const float rest_density = 50.0f;
	SOARepository* repository_;
	SpatialHashGrid* grid_;
	std::map<std::pair<unsigned int, unsigned int>, float> distance_cache_;
	
	IncompressibilityConstraint(SOARepository* repository, SpatialHashGrid* grid) : repository_(repository), grid_(grid) {};

	void CalculateLagrangeMultiplier();
	void CalculatePositionCorrections();
	void CalculateNewVelocities();
	void ApplyPositionCorrection();
	void ApplyNewVelocities();
};

