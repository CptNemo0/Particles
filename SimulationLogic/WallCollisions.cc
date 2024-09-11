#include "pch.h"
#include "WallCollisions.h"
#include "utility.h"
#include "algorithm"
#include <omp.h>


void WallCollisions2D::Init()
{

}

void WallCollisions2D::CollideWalls(SOARepository& repository)
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

void WallCollisions3D::Init()
{
}

void WallCollisions3D::CollideWalls(SOARepository& repository)
{
	int n = static_cast<int>(repository.size_);

	for (int i = 0; i < n; i++)
	{
		float& x = repository.x_[i];
		float& y = repository.y_[i];
		float& z = repository.z_[i];
		float& nx = repository.nx_[i];
		float& ny = repository.ny_[i];
		float& nz = repository.nz_[i];
		float& speedx = repository.speedx_[i];
		float& speedy = repository.speedy_[i];
		float& speedz = repository.speedz_[i];
		float& radius = repository.radius_[i];

		const float RIGHT_MRADIUS = right_ - radius;
		const float DOWN_MRADIUS  = down_  - radius;
		const float NEAR_MRADIUS  = near_  - radius;
		const float LEFT_ARADIUS  = left_  + radius;
		const float TOP_ARADIUS   = top_   + radius;
		const float FAR_ARADIUS   = far_   + radius;
		
		const float COR = 0.98f;

		if (nx <= LEFT_ARADIUS)
		{
			auto dx = x - nx;
			auto offset = LEFT_ARADIUS - nx;
			nx += offset;
			x = (nx - dx) * COR;
		}
		else if (nx >= RIGHT_MRADIUS)
		{
			auto dx = nx -x;
			auto offset = nx - RIGHT_MRADIUS;
			nx -= offset;
			x = (nx + dx) * COR;
		}

		if (ny <= TOP_ARADIUS)
		{
			auto dy = y - ny;
			auto offset = TOP_ARADIUS - ny;
			ny += offset;
			y = (ny - dy) * COR;
		}
		else if (ny >= DOWN_MRADIUS)
		{
			auto dy = ny - y;
			auto offset = ny - DOWN_MRADIUS;
			ny -= offset;
			y = (ny + dy) * COR;
		}

		if (nz <= FAR_ARADIUS)
		{
			auto dz = z - nz;
			auto offset = FAR_ARADIUS - nz;
			nz += offset;
			z = (nz - dz) * COR;
		}
		else if (nz >= NEAR_MRADIUS)
		{
			auto dz = nz - z;
			auto offset = nz - NEAR_MRADIUS;
			nz -= offset;
			z = (nz + dz) * COR;
		}
	}
}
