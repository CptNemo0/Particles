#include "pch.h"
#include "BallCollisions.h"
#include <iostream>
void BallCollisions::SeperateBalls(SpatialHashGrid& grid, SOARepository& repository)
{
	for (int id = 0; id < static_cast<int>(repository.size_); id++)
	{
		float& x = repository.nx_[id];
		float& y = repository.ny_[id];

		int cellx = static_cast<int>(x / CELL_SIZE);
		int celly = static_cast<int>(y / CELL_SIZE);

		float radius_squared = repository.radius_[id] * repository.radius_[id];
		float radius2 = repository.radius_[id] * 2;

		for (const auto [offsetx, offsety] : offsets_2d)
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
					if (distance2 < 0.0000001)
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
