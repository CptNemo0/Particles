#pragma once

#include "framework.h"
#include "global.h"
#include "glm/glm.hpp"

enum DIMENSIONS
{
	D2,
	D3
};

class SOARepository
{
	public:
		alignas(32) float px_[BALL_NUMBER];
		alignas(32) float py_[BALL_NUMBER];
		alignas(32) float pz_[BALL_NUMBER];
		
		alignas(32) float x_[BALL_NUMBER];
		alignas(32) float y_[BALL_NUMBER];
		alignas(32) float z_[BALL_NUMBER];

		alignas(32) float nx_[BALL_NUMBER];
		alignas(32) float ny_[BALL_NUMBER];
		alignas(32) float nz_[BALL_NUMBER];

		alignas(32) float dx_[BALL_NUMBER];
		alignas(32) float dy_[BALL_NUMBER];
		alignas(32) float dz_[BALL_NUMBER];

		alignas(32) float speedx_[BALL_NUMBER];
		alignas(32) float speedy_[BALL_NUMBER];
		alignas(32) float speedz_[BALL_NUMBER];

		alignas(32) float pspeedx_[BALL_NUMBER];
		alignas(32) float pspeedy_[BALL_NUMBER];
		alignas(32) float pspeedz_[BALL_NUMBER];

		alignas(32) float forcex_[BALL_NUMBER];
		alignas(32) float forcey_[BALL_NUMBER];
		alignas(32) float forcez_[BALL_NUMBER];

		alignas(32) float mass_[BALL_NUMBER];
		alignas(32) float inv_mass_[BALL_NUMBER];

		alignas(32) float density_[BALL_NUMBER];
		alignas(32) glm::vec3 gradient_[BALL_NUMBER];
		alignas(32) float lagrange_multiplier_[BALL_NUMBER];

		alignas(32) unsigned int id_[BALL_NUMBER];
		alignas(32) float radius_[BALL_NUMBER];
		alignas(32) float output_position_[BALL_NUMBER * 3];

		unsigned int size_;

		SOARepository(const unsigned int size = 0, const DIMENSIONS dim = D2);
};