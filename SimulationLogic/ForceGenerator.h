#pragma once

#include "BallRepository.h"
#include "glm/glm.hpp"
#include <vector>
#include <omp.h>

class ForceGenerator
{
public:
	SOARepository* repository_;
	std::vector<glm::vec3> constant_forces_;
	
	ForceGenerator(SOARepository* repository) : repository_(repository) {};
	void AddForce(const glm::vec3& force);
	void ApplyForces();
	void ZeroForces();
};

