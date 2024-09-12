#include "pch.h"
#include "ForceGenerator.h"

void ForceGenerator::AddForce(const glm::vec3& force)
{
	constant_forces_.push_back(force);
}

void ForceGenerator::ApplyForces()
{
	for (auto& force : constant_forces_)
	{
#pragma omp parallel for
		for (int i = 0; i < repository_->size_; i++)
		{
			repository_->forcex_[i] = force.x;
			repository_->forcey_[i] = force.y;
			repository_->forcez_[i] = force.z;
		}
	}
}

void ForceGenerator::ZeroForces()
{
	for (int i = 0; i < repository_->size_; i++)
	{
		repository_->forcex_[i] = 0.0f;
		repository_->forcey_[i] = 0.0f;
		repository_->forcez_[i] = 0.0f;
	}
}