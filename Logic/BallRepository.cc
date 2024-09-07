#include "pch.h"
#include <random>

SOARepository::SOARepository(const unsigned int size)
{
	assert(!(size % SIMD_BLOCK_SIZE));

	srand(time(NULL));

	this->size_ = size;

	int particles_per_row = sqrt(BALL_NUMBER);
	int particles_per_column = (BALL_NUMBER - 1) / particles_per_row + 1;
	int spacex = RADIUS * 2 + 5.0f;
	int spacey = RADIUS * 2 + 2.0f;

	for (unsigned int i = 0; i < size_; i++)
	{
		id_[i] = i;

		x_[i] = (i % particles_per_row - particles_per_row / 2.0f + 0.5f) * spacex + WINDOW_WIDTH * 0.5f;
		y_[i] = (i / particles_per_row - particles_per_column / 2.0f + 0.5f) * spacey +WINDOW_HEIGHT * 0.5f;

		speedx_[i] = static_cast<float>(rand() % 1000 - 0) / 1000.0f;
		speedy_[i] = static_cast<float>(rand() % 1000 - 1000) / 1000.0f;

		speedx_[i] *= 300.0f;
		speedy_[i] *= 0.0f;


		px_[i] = x_[i] - speedx_[i];
		py_[i] = y_[i] - speedy_[i];

		nx_[i] = x_[i];
		ny_[i] = y_[i];

		mass[i] = 1.0f;
		inv_mass[i] = 1.0f;

		forcex_[i] = 0.0f;
		forcey_[i] = 0.0f;

		radius_[i] = RADIUS;
	}
}
