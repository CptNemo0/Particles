#include "pch.h"
#include "IncompressibilityConstraint.h"

void IncompressibilityConstraint::CalculateLagrangeMultiplier()
{
	for (int id = 0; id < static_cast<int>(repository_->size_); id++)
	{
		repository_->gradient_[id] = glm::vec3(0.0f);
		repository_->density_[id] = 0.0f;
		float sum_squared_gradients = 0.0f;

		float& x = repository_->nx_[id];
		float& y = repository_->ny_[id];
		float& z = repository_->nz_[id];

		for (const auto& idx : grid_->neighbors_[id])
		{
			float& ox = repository_->nx_[idx];
			float& oy = repository_->ny_[idx];
			float& oz = repository_->nz_[idx];

			float displacement_x = x - ox;
			float displacement_y = y - oy;
			float displacement_z = z - oz;

			float distance_squared = displacement_x * displacement_x + displacement_y * displacement_y + displacement_z * displacement_z;
			float distance = sqrtf(distance_squared);
			glm::vec3 direction = glm::vec3(displacement_x, displacement_y, displacement_z) / (distance + 1.0f);

			repository_->density_[id] += poly6(distance);
			
			glm::vec3 gradient_idx =  direction * spiky(distance) / rest_density;
			repository_->gradient_[id] += gradient_idx;
			sum_squared_gradients += glm::dot(gradient_idx, gradient_idx);
		}

		//if (id == 100)
		//{
		//	std::cout << sum_squared_gradients << std::endl;
		//	std::cout << repository_->gradient_[id].x << " " << repository_->gradient_[id].y << " " << repository_->gradient_[id].z << " \n";
		//}

		float density_constraint = (repository_->density_[id] / rest_density) - 1.0f;
		sum_squared_gradients += glm::dot(repository_->gradient_[id], repository_->gradient_[id]);
		
		repository_->lagrange_multiplier_[id] = (-1.0f * density_constraint) / (sum_squared_gradients + 100.0f);
	}
}

void IncompressibilityConstraint::CalculatePositionCorrections()
{
	for (int id = 0; id < static_cast<int>(repository_->size_); id++)
	{
		glm::vec3 dp(0.0f); //= repository_->gradient_[id] * repository_->lagrange_multiplier_[id];

		repository_->dx_[id] = 0.0f;
		repository_->dy_[id] = 0.0f;
		repository_->dz_[id] = 0.0f;
		
		float& x = repository_->nx_[id];
		float& y = repository_->ny_[id];
		float& z = repository_->nz_[id];
		
		for (const auto& idx : grid_->neighbors_[id])
		{
			float& ox = repository_->nx_[idx];
			float& oy = repository_->ny_[idx];
			float& oz = repository_->nz_[idx];
		
			float displacement_x = x - ox;
			float displacement_y = y - oy;
			float displacement_z = z - oz;
		
			float distance_squared = displacement_x * displacement_x + displacement_y * displacement_y + displacement_z * displacement_z;
			float distance = sqrtf(distance_squared);
			glm::vec3 direction = glm::vec3(displacement_x, displacement_y, displacement_z) / (distance + 0.000001f);
		
			float lagrange_sum = repository_->lagrange_multiplier_[id] + repository_->lagrange_multiplier_[idx];
			float s_corr = poly6(sqrtf(distance)) / poly6(0.3f * INFLUENCE_RADIUS);
			s_corr = -1.0f * 0.1f * s_corr * s_corr * s_corr * s_corr;
			lagrange_sum += s_corr;
			glm::vec3 gradient = direction * spiky(distance);
		
			dp += gradient * lagrange_sum;
		}
		
		dp /= rest_density;

		//if (id == 100)
		//{
		//	std::cout << repository_->gradient_[id].x << " " << repository_->gradient_[id].y << " " << repository_->gradient_[id].z << " \n";
		//}

		repository_->dx_[id] += dp.x;
		repository_->dy_[id] += dp.y;
		repository_->dz_[id] += dp.z;
	}
}

void IncompressibilityConstraint::ApplyPositionCorrection()
{
	for (int id = 0; id < static_cast<int>(repository_->size_); id++)
	{
		repository_->nx_[id] += repository_->dx_[id];
		repository_->ny_[id] += repository_->dy_[id];
		repository_->nz_[id] += repository_->dz_[id];
	}
}

void IncompressibilityConstraint::CalculateNewVelocities()
{
	for (int id = 0; id < static_cast<int>(repository_->size_); id++)
	{
		glm::vec3 p(repository_->nx_[id], repository_->ny_[id], repository_->nz_[id]);

		float sum_x = 0.0f;
		float sum_y = 0.0f;
		float sum_z = 0.0f;

		for (const auto& idx : grid_->neighbors_[id])
		{
			glm::vec3 op(repository_->nx_[idx], repository_->ny_[idx], repository_->nz_[idx]);
			
			float distance = glm::distance(op, p);
			float kerneled = poly6(distance);
			sum_x = (repository_->speedx_[id] - repository_->speedx_[idx]) * kerneled;
			sum_y = (repository_->speedy_[id] - repository_->speedy_[idx]) * kerneled;
			sum_z = (repository_->speedz_[id] - repository_->speedz_[idx]) * kerneled;
		}

		sum_x *= 0.01f;
		sum_y *= 0.01f;
		sum_z *= 0.01f;

		repository_->pspeedx_[id] = repository_->speedx_[id] + sum_x;
		repository_->pspeedy_[id] = repository_->speedy_[id] + sum_y;
		repository_->pspeedz_[id] = repository_->speedz_[id] + sum_z;
	}
}

void IncompressibilityConstraint::ApplyNewVelocities()
{
	for (int id = 0; id < static_cast<int>(repository_->size_); id++)
	{
		repository_->speedx_[id] = repository_->pspeedx_[id];
		repository_->speedy_[id] = repository_->pspeedy_[id];
		repository_->speedz_[id] = repository_->pspeedz_[id];
	}
}