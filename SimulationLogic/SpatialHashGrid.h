#pragma once

#include "framework.h"

#include "BallRepository.h"
#include "global.h"
#include <list>
#include <vector>

class SpatialHashGrid 
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

	SOARepository* repository_;

	

public:

	SpatialHashGrid(SOARepository* repository) : repository_(repository) {};
	~SpatialHashGrid() = default;
	
	unsigned int spatial_lookup_[BALL_NUMBER][2];
	unsigned int start_indices_[BALL_NUMBER];
	std::vector<unsigned int> neighbors_[BALL_NUMBER];
	int Partition(unsigned int arr[][2], int low, int high);
	void QuickSrot(unsigned int arr[BALL_NUMBER][2], int low, int high);
	void UpdateGrid();
	void UpdateNeighbors();
};

