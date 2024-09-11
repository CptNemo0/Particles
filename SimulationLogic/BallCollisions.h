#pragma once

#include "framework.h"
#include "SpatialHashGrid.h"
#include "BallRepository.h"
#include "vector"

class BallCollisions
{
protected:
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

	virtual void SeperateBalls(SOARepository& repository) = 0;
	virtual void SeperateBalls(SpatialHashGrid& grid, SOARepository& repository) = 0;
};

class BallCollisions2d : public BallCollisions
{
public:
	BallCollisions2d() = default;
	
	void SeperateBalls(SOARepository& repository) override;
	void SeperateBalls(SpatialHashGrid& grid, SOARepository& repository) override;
};

class BallCollisions3d : public BallCollisions
{
public:
	BallCollisions3d() = default;
	
	void SeperateBalls(SOARepository& repository) override;
	void SeperateBalls(SpatialHashGrid& grid, SOARepository& repository) override;
};