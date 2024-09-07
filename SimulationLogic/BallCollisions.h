#pragma once

#include "framework.h"
#include "SpatialHashGrid.h"
#include "BallRepository.h"

class BallCollisions
{
public:
	BallCollisions() = default;

	void SeperateBalls(SpatialHashGrid& grid, SOARepository& repository);
};

