#pragma once

#include "framework.h"

#include "BallRepository.h"
#include "global.h"

class SpatialHashGrid 
{
public:
	SpatialHashGrid();
	~SpatialHashGrid() = default;
	
	unsigned int spatial_lookup_[BALL_NUMBER][2];
	unsigned int start_indices_[BALL_NUMBER];

	void UpdateGrid(SOARepository& repository);

private:
	int Partition(unsigned int arr[][2], int low, int high);

	void QuickSrot(unsigned int arr[BALL_NUMBER][2], int low, int high);
};

