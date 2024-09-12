#include "pch.h"
#include "BallCollisions.h"
#include <iostream>

void BallCollisions2d::SeperateBalls(SOARepository& repository)
{
	for (int i = 0; i < static_cast<int>(repository.size_) - 1; i++)
	{
		for (int j = i + 1; j < static_cast<int>(repository.size_); j++)
		{
			float& x = repository.nx_[i];
			float& y = repository.ny_[i];

			float distance2 = (repository.nx_[j] - x) * (repository.nx_[j] - x) + (repository.ny_[j] - y) * (repository.ny_[j] - y);

			float min_distance = repository.radius_[j] + repository.radius_[i];

			if (distance2 < (min_distance * min_distance))
			{
				if (distance2 < 0.0000001)
				{
					distance2 = 0.00001f;
				}

				float distance = sqrtf(distance2);

				glm::vec2 particleA(x, y);
				glm::vec2 particleB(repository.nx_[j], repository.ny_[j]);

				glm::vec2 dir = glm::normalize(particleA - particleB);
				float overlap = min_distance - distance;

				float move_factor_1 = repository.radius_[j] / min_distance;
				float move_factor_2 = repository.radius_[i] / min_distance;

				glm::vec2 displacement_1 = dir * overlap * move_factor_1;
				glm::vec2 displacement_2 = dir * overlap * move_factor_2;

				repository.nx_[j] -= displacement_1.x;
				repository.ny_[j] -= displacement_1.y;

				repository.nx_[i] += displacement_2.x;
				repository.ny_[i] += displacement_2.y;

				repository.speedx_[i] *= 0.999f;
				repository.speedy_[i] *= 0.999f;
				repository.speedx_[j] *= 0.999f;
				repository.speedy_[j] *= 0.999f;

			}
		}
	}
}

void BallCollisions2d::SeperateBalls(SpatialHashGrid& grid, SOARepository& repository)
{
	for (int id = 0; id < static_cast<int>(repository.size_); id++)
	{
		float& x = repository.nx_[id];
		float& y = repository.ny_[id];

		int cellx = static_cast<int>(x / CELL_SIZE);
		int celly = static_cast<int>(y / CELL_SIZE);

		float radius_squared = repository.radius_[id] * repository.radius_[id];
		float radius2 = repository.radius_[id] * 2.0f;

		for (const auto& [offsetx, offsety] : offsets_2d)
		{
			unsigned int key = hash2uints(cellx + offsetx, celly + offsety) % repository.size_;
			unsigned int start = grid.start_indices_[key];

			unsigned int i = 0;
			while (true)
			{
				if (start + i >= repository.size_ || grid.spatial_lookup_[start + i][0] != key)
				{
					break;
				}

				unsigned int idx = grid.spatial_lookup_[start + i][1];

				if (idx == id)
				{
					i++;
					continue;
				}

				float distance2 = (repository.nx_[idx] - x) * (repository.nx_[idx] - x) + (repository.ny_[idx] - y) * (repository.ny_[idx] - y);

				float min_distance = repository.radius_[idx] + repository.radius_[id];

				if (distance2 < (min_distance * min_distance))
				{
					if (distance2 < 0.0000001f)
					{
						distance2 = 0.00001f;
					}

					float distance = sqrtf(distance2);

					glm::vec2 particleA(x, y);
					glm::vec2 particleB(repository.nx_[idx], repository.ny_[idx]);

					glm::vec2 dir = glm::normalize(particleA - particleB);
					float overlap = min_distance - distance;

					float move_factor_1 = repository.radius_[idx] / min_distance;
					float move_factor_2 = repository.radius_[id] / min_distance;

					glm::vec2 displacement_1 = dir * overlap * move_factor_1;
					glm::vec2 displacement_2 = dir * overlap * move_factor_2;

					repository.nx_[idx] -= displacement_1.x;
					repository.ny_[idx] -= displacement_1.y;

					repository.nx_[id] += displacement_2.x;
					repository.ny_[id] += displacement_2.y;

					repository.speedx_[id] *= 0.999f;
					repository.speedy_[id] *= 0.999f;
					repository.speedx_[idx] *= 0.999f;
					repository.speedy_[idx] *= 0.999f;

				}
				i++;
			}
		}
	}
}

void BallCollisions3d::SeperateBalls(SOARepository& repository)
{

}

void BallCollisions3d::SeperateBalls(SpatialHashGrid& grid, SOARepository& repository)
{
	for (int id = 0; id < static_cast<int>(repository.size_); id++)
	{
		float& x = repository.nx_[id];
		float& y = repository.ny_[id];
		float& z = repository.nz_[id];

		int cellx = static_cast<int>(x / CELL_SIZE);
		int celly = static_cast<int>(y / CELL_SIZE);
		int cellz = static_cast<int>(z / CELL_SIZE);

		float radius_squared = repository.radius_[id] * repository.radius_[id];
		float radius2 = repository.radius_[id] * 2.0f;

		for (const auto& [offsetx, offsety, offsetz] : offsets_3d)
		{
			unsigned int key = hash3uints(cellx + offsetx, celly + offsety, cellz + offsetz) % repository.size_;
			unsigned int start = grid.start_indices_[key];

			unsigned int i = 0;
			while (true)
			{
				if (start + i >= repository.size_ || grid.spatial_lookup_[start + i][0] != key)
				{
					break;
				}

				unsigned int idx = grid.spatial_lookup_[start + i][1];

				if (idx == id)
				{
					i++;
					continue;
				}

				float& ox = repository.nx_[idx];
				float& oy = repository.ny_[idx];
				float& oz = repository.nz_[idx];

				float distance2 = (ox - x) * (ox - x) + (oy - y) * (oy - y) + (oz - z) * (oz - z);

				float min_distance = repository.radius_[idx] + repository.radius_[id];

				if (distance2 < (min_distance * min_distance))
				{
					if (distance2 < 0.0000001f)
					{
						distance2 = 0.00001f;
					}

					float distance = sqrtf(distance2);

					glm::vec3 particleA(x, y, z);
					glm::vec3 particleB(ox, oy, oz);

					glm::vec3 dir = glm::normalize(particleA - particleB);
					float overlap = min_distance - distance;

					float move_factor_1 = repository.radius_[idx] / min_distance;
					float move_factor_2 = repository.radius_[id] / min_distance;

					glm::vec3 displacement_1 = dir * overlap * move_factor_1;
					glm::vec3 displacement_2 = dir * overlap * move_factor_2;

					ox -= displacement_1.x;
					oy -= displacement_1.y;
					oz -= displacement_1.z;

					x += displacement_2.x;
					y += displacement_2.y;
					z += displacement_2.z;
				}
				i++;
			}
		}
	}
}
