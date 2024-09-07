#pragma once

#include "framework.h"
#include "SpatialHashGrid.h"
#include "BallRepository.h"
#include "vector"

class BallCollisions
{
private:
	std::vector<std::pair<int, int>> offsets_2d
	{
		{-1, -1},
		{-1,  0},
		{-1,  1},

		{ 0, -1},
		{ 0,  0},
		{ 0,  1},

		{ 1, -1},
		{ 1,  0},
		{ 1,  1},
	};

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
	BallCollisions() = default;

	void SeperateBalls(SpatialHashGrid& grid, SOARepository& repository);
};

