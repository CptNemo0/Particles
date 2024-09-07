#include "pch.h"
#include "DensityEstimator.h"
#include <iostream>

DensityEstimator::DensityEstimator(int a)
{
	float w = WINDOW_WIDTH / CELL_SIZE;
	float h = WINDOW_WIDTH / CELL_SIZE;

	float wh = w * h;
	
	float d1 = BALL_NUMBER / wh;

	target_density = GetInfluence(INFLUENCE_RADIUS, d1);
	std::cout << target_density << std::endl;

}

void DensityEstimator::EstimateDensity(SOARepository& repository, const SpatialHashGrid& grid)
{
	for (unsigned int i = 0; i < repository.size_; i++)
	{
		repository.density_[i] = GetDensity(repository.id_[i],
												   repository,
												   grid);
	}
}

void DensityEstimator::EstimatePressure(SOARepository& repository, const SpatialHashGrid& grid)
{
	for (unsigned int i = 0; i < repository.size_; i++)
	{
		glm::vec2 f = GetPressure(repository.id_[i],
										repository,
										grid);
		repository.forcex_[i] = f.x;
		repository.forcey_[i] = f.y;
	}
}

float DensityEstimator::GetDensity(unsigned int id, SOARepository& repository, const SpatialHashGrid& grid)
{
	float return_value = 0;

	float x = repository.nx_[id];
	float y = repository.ny_[id];

	int cellx = static_cast<int>(x / CELL_SIZE);
	int celly = static_cast<int>(y / CELL_SIZE);

	float iradius = repository.radius_[id];
	float radius2 = iradius * iradius;

	for (int offsetx = -1; offsetx < 2; offsetx++)
	{
		for (int offsety = -1; offsety < 2; offsety++)
		{
			unsigned int key = hash2uints(cellx + offsetx, celly + offsety) % repository.size_;
			unsigned int start = grid.start_indices_[key];

			for (int i = start; i < repository.size_; i++)
			{
				if (i >= repository.size_)
				{
					break;
				}

				if (grid.spatial_lookup_[i][0] != key)
				{
					break;
				}

				if (grid.spatial_lookup_[i][1] == id)
				{
					continue;
				}

				auto other_id = grid.spatial_lookup_[i][1];

				glm::vec2 this_(x, y);
				glm::vec2 other_(repository.nx_[other_id], repository.ny_[other_id]);
					
				float distance = glm::length(this_ - other_);
				
				if (distance < iradius)
				{
					return_value += GetInfluence(iradius, distance);
				}
			}
		}
	}
	
	if (return_value < 0.0000001f)
	{
		return_value = 0.0000001f;
	}
	return return_value;
}

glm::vec2 DensityEstimator::GetPressure(unsigned int id, SOARepository& repository, const SpatialHashGrid& grid)
{
	glm::vec2 return_value (0.0f);

	float x = repository.nx_[id];
	float y = repository.ny_[id];

	int cellx = static_cast<int>(x / CELL_SIZE);
	int celly = static_cast<int>(y / CELL_SIZE);

	float iradius = repository.radius_[id];
	float radius2 = iradius * iradius;

	for (int offsetx = -1; offsetx < 2; offsetx++)
	{
		for (int offsety = -1; offsety < 2; offsety++)
		{
			unsigned int key = hash2uints(cellx + offsetx, celly + offsety) % repository.size_;
			unsigned int start = grid.start_indices_[key];

			for (int i = start; i < repository.size_; i++)
			{
				if (i >= repository.size_)
				{
					break;
				}

				if (grid.spatial_lookup_[i][0] != key)
				{
					break;
				}

				if (grid.spatial_lookup_[i][1] == id)
				{
					continue;
				}

				auto other_id = grid.spatial_lookup_[i][1];

				glm::vec2 this_(x, y);
				glm::vec2 other_(repository.nx_[other_id], repository.ny_[other_id]);

				float distance = glm::length(this_ - other_);

				if (distance < iradius)
				{
					float dirx = (x - repository.nx_[i]) / distance;
					float diry = (y - repository.ny_[i]) / distance;
					float density = repository.density_[i];
					float influence = GetInfluenceDerivative(iradius, distance);
					float iod = influence / density;
					float pressure = (DensityToPressure(density) + DensityToPressure(repository.density_[id])) * 0.5f;
					float iodp = iod * pressure;
					return_value.x += iodp * dirx;
					return_value.y += iodp * diry;
				}
			}
		}
	}

	return return_value;
}

float DensityEstimator::GetInfluence(float radius, float distance)
{
	if (distance >= radius)
	{
		return 0;
	}

	float volume = 3.14159f * std::powf(radius, 4) * 0.1666666666f;
	return (radius - distance) * (radius - distance) / volume;
}

float DensityEstimator::GetInfluenceDerivative(float radius, float distance)
{
	if (distance >= radius)
	{
		return 0;
	}

	float scale = 12.0f / (3.14159f * std::powf(radius, 4));
	return (distance - radius) * scale;
}

float DensityEstimator::DensityToPressure(float density)
{
	float difference = density - target_density;
	float preassure = difference * preassure_factor;
	return preassure;
}
