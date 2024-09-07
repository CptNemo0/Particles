#include "pch.h"
#include "WallCollisions.h"
#include "utility.h"
#include "algorithm"
#include <omp.h>


void LinearWallCollisions::Init()
{

}

void LinearWallCollisions::CollideWalls(SOARepository& repository)
{
	int n = static_cast<int>(repository.size_);

	for (int i = 0; i < n; i++)
	{
		float& nx = repository.nx_[i];
		float& ny = repository.ny_[i];
		float& speedx = repository.speedx_[i];
		float& speedy = repository.speedy_[i];
		float& radius = repository.radius_[i];
		
		const float RIGHT_MRADIUS = right_ - radius * 2.0f;
		const float DOWN_MRADIUS  = down_  - radius * 2.0f;
		const float LEFT_ARADIUS  = left_  + radius * 2.0f;
		const float TOP_ARADIUS   = top_   + radius * 2.0f;

		float distance = 0.0f;

		distance = nx - LEFT_ARADIUS;
		distance = std::min(0.0f, distance);
		distance = fabsf(distance);
		repository.nx_[i] += distance;

		if (distance > 0)
		{
			repository.speedx_[i] *= -0.9f;
		}

		distance = ny - TOP_ARADIUS;
		distance = std::min(0.0f, distance);
		distance = fabsf(distance);
		repository.ny_[i] += distance;
		
		if (distance > 0)
		{
			repository.speedy_[i] *= -0.9f;
		}
		
		distance = RIGHT_MRADIUS - nx;
		distance = std::min(0.0f, distance);
		repository.nx_[i] += distance;
		
		if (distance < 0)
		{
			repository.speedx_[i] *= -0.9f;
		}
		
		distance = DOWN_MRADIUS - ny;
		distance = std::min(0.0f, distance);
		repository.ny_[i] += distance;
		
		if (distance < 0)
		{
			repository.speedy_[i] *= -0.9f;
		}
	}
}


