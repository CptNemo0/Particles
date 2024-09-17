#include "pch.h"
#include "IncompressibilityConstraint.h"

void IncompressibilityConstraint::CalculateLagrangeMultiplier()
{
	float relaxation_parameter = RELAXATION_PARAMETER;
	float inv_rest_density = INV_REST_DENSITY;

#pragma omp parallel for firstprivate(relaxation_parameter, inv_rest_density)
	for (int i = 0; i < static_cast<int>(repository_->size_); i++)
	{
		glm::vec3 p_i(repository_->nx_[i], repository_->ny_[i], repository_->nz_[i]);
		glm::vec3 gradient_i(0.0f);
		float sum_of_squares = 0.0f;
		repository_->density_[i] = 0.0f;

		for (const auto& j : grid_->neighbors_[i])
		{
			glm::vec3 p_j(repository_->nx_[j], repository_->ny_[j], repository_->nz_[j]);

			glm::vec3 p_ij = p_i - p_j;
			float magnitude = glm::length(p_ij);

			if (magnitude < 0.000001f)
			{
				continue;
			}

			repository_->density_[i] += poly6(magnitude);
			glm::vec3 gradient_j = spiky_gradient(p_ij, magnitude);
			gradient_i += gradient_j;
			sum_of_squares += glm::dot(gradient_j, gradient_j);
		}

		float constraint = (repository_->density_[i] * inv_rest_density) - 1.0f;
		sum_of_squares += glm::dot(gradient_i, gradient_i);
		repository_->lagrange_multiplier_[i] = -1.0f * (constraint / (sum_of_squares + relaxation_parameter));
	}
}

void IncompressibilityConstraint::CalculatePositionCorrections()
{
	float influence_radius = INFLUENCE_RADIUS;
	float inv_rest_density = INV_REST_DENSITY;
#pragma omp parallel for firstprivate(inv_rest_density, influence_radius)
	for (int i = 0; i < static_cast<int>(repository_->size_); i++)
	{
		glm::vec3 p_i(repository_->nx_[i], repository_->ny_[i], repository_->nz_[i]);
		glm::vec3 offset = glm::vec3(0.0f);
		for (const auto& j : grid_->neighbors_[i])
		{
			glm::vec3 p_j(repository_->nx_[j], repository_->ny_[j], repository_->nz_[j]);

			glm::vec3 p_ij = p_i - p_j;
			float magnitude = glm::length(p_ij);
			glm::vec3 gradient = spiky_gradient(p_ij, magnitude);
			float smoothing_kernel = (poly6(magnitude) / poly6(0.3f * influence_radius));
			smoothing_kernel = smoothing_kernel * smoothing_kernel * smoothing_kernel * smoothing_kernel;
			smoothing_kernel *= -0.002f;
			float lagrange_sum = repository_->lagrange_multiplier_[i] + repository_->lagrange_multiplier_[j] + smoothing_kernel;
			offset += gradient * lagrange_sum;
		}

		offset *= inv_rest_density;
		repository_->dx_[i] = offset.x;
		repository_->dy_[i] = offset.y;
		repository_->dz_[i] = offset.z;
	}
}

void IncompressibilityConstraint::ApplyPositionCorrection()
{
#pragma omp parallel for
	for (int i = 0; i < static_cast<int>(repository_->size_); i++)
	{
		repository_->nx_[i] += repository_->dx_[i];
		repository_->ny_[i] += repository_->dy_[i];
		repository_->nz_[i] += repository_->dz_[i];
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

		sum_x *= 0.1f;
		sum_y *= 0.1f;
		sum_z *= 0.1f;

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

void IncompressibilityConstraint::UpdateAndVicosity()
{
	float inv_dixed_dt = INV_FIXED_DT;
#pragma omp parallel for firstprivate(inv_dixed_dt)
	for (int i = 0; i < static_cast<int>(repository_->size_); i++)
	{
		repository_->speedx_[i] = inv_dixed_dt * (repository_->nx_[i] - repository_->x_[i]);
		repository_->speedy_[i] = inv_dixed_dt * (repository_->ny_[i] - repository_->y_[i]);
		repository_->speedz_[i] = inv_dixed_dt * (repository_->nz_[i] - repository_->z_[i]);

		glm::vec3 p_i(repository_->nx_[i], repository_->ny_[i], repository_->nz_[i]);
		glm::vec3 v_i(repository_->speedx_[i], repository_->speedy_[i], repository_->speedz_[i]);
		glm::vec3 new_v(0.0f);
		glm::vec3 vorticity_sum(0.0f);

		for (const auto& j : grid_->neighbors_[i])
		{
			glm::vec3 p_j(repository_->nx_[j], repository_->ny_[j], repository_->nz_[j]);
			glm::vec3 v_j(repository_->speedx_[j], repository_->speedy_[j], repository_->speedz_[j]);

			glm::vec3 p_ij = p_i - p_j;
			glm::vec3 v_ij = v_i - v_j;

			float magnitude = glm::length(p_ij);

			vorticity_sum += glm::cross(v_ij, spiky_gradient(p_ij, magnitude));

			new_v += poly6(magnitude) * v_ij;
		}

		new_v *= 0.01f;

		repository_->speedx_[i] = repository_->speedx_[i] + new_v.x;
		repository_->speedy_[i] = repository_->speedy_[i] + new_v.y;
		repository_->speedz_[i] = repository_->speedz_[i] + new_v.z;

		repository_->x_[i] = repository_->nx_[i];
		repository_->y_[i] = repository_->ny_[i];
		repository_->z_[i] = repository_->nz_[i];

		repository_->output_position_[i * 3 + 0] = repository_->x_[i];
		repository_->output_position_[i * 3 + 1] = repository_->y_[i];
		repository_->output_position_[i * 3 + 2] = repository_->z_[i];
	}
}
