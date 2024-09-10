#pragma once

#include "framework.h"
#include "global.h"

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

		alignas(32) float speedx_[BALL_NUMBER];
		alignas(32) float speedy_[BALL_NUMBER];
		alignas(32) float speedz_[BALL_NUMBER];

		alignas(32) float forcex_[BALL_NUMBER];
		alignas(32) float forcey_[BALL_NUMBER];
		alignas(32) float forcez_[BALL_NUMBER];

		alignas(32) float mass[BALL_NUMBER];
		alignas(32) float inv_mass[BALL_NUMBER];

		alignas(32) unsigned int id_[BALL_NUMBER];
		alignas(32) float radius_[BALL_NUMBER];
		alignas(32) float output_position_[BALL_NUMBER * 3];

		unsigned int size_;

		SOARepository(const unsigned int size = 0, const DIMENSIONS dim = D2);
};