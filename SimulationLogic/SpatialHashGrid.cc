#include "SpatialHashGrid.h"
#include <omp.h>
#include "pch.h"
#include <algorithm>

void SpatialHashGrid::UpdateGrid()
{
	int n = repository_->size_;

#pragma omp parallel for
	for (int i = 0; i < n; i++)
	{
        unsigned int cellx = static_cast<unsigned int>(repository_->nx_[i] / CELL_SIZE);
        unsigned int celly = static_cast<unsigned int>(repository_->ny_[i] / CELL_SIZE);
        unsigned int cellz = static_cast<unsigned int>(repository_->nz_[i] / CELL_SIZE);
		unsigned int hash = hash3uints(cellx, celly, cellz);
        unsigned int key = hash % n;
		spatial_lookup_[i][0] = key;
		spatial_lookup_[i][1] = i;
		start_indices_[i] = 4294967295;
	}

	QuickSrot(spatial_lookup_, 0, n - 1);

    if (spatial_lookup_[0][0] != 4294967295)
    {
        start_indices_[spatial_lookup_[0][0]] = 0;
    }
#pragma omp parallel for
    for (int i = 1; i < n; i++)
    {
        if (spatial_lookup_[i][0] != spatial_lookup_[i - 1][0])
        {
            start_indices_[spatial_lookup_[i][0]] = i;
        }
    }
}

int SpatialHashGrid::Partition(unsigned int arr[][2], int low, int high)
{

    unsigned int pivot = arr[high][0];

    int i = low - 1;

    for (int j = low; j <= high - 1; j++)
    {
        if (arr[j][0] < pivot)
        {
            i++;
            std::swap(arr[i], arr[j]);
            
        }
    }
    std::swap(arr[i + 1], arr[high]);

    return i + 1;
}

void SpatialHashGrid::QuickSrot(unsigned int arr[BALL_NUMBER][2], int low, int high) {

    if (low < high)
    {
        int pi = Partition(arr, low, high);

        QuickSrot(arr, low, pi - 1);
        QuickSrot(arr, pi + 1, high);
    }
}
