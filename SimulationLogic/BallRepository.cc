#include "pch.h"
#include <random>
#include <ctime>

SOARepository::SOARepository(const unsigned int size, const DIMENSIONS dim)
{
	assert(!(size % SIMD_BLOCK_SIZE));

	srand(static_cast<unsigned int>(time(0)));
	size_ = size;

	switch (dim)
	{
		case DIMENSIONS::D2:
		{
			int particles_per_row = static_cast<int>(sqrt(BALL_NUMBER));
			int particles_per_column = (BALL_NUMBER - 1) / particles_per_row + 1;
			int spacex = static_cast<int>(RADIUS * 2.0f + 0.1f);
			int spacey = static_cast<int>(RADIUS * 2.0f + 0.1f);

			for (unsigned int i = 0; i < size_; i++)
			{
				id_[i] = i;

				x_[i] = (i % particles_per_row - particles_per_row / 2.0f + 0.5f) * spacex + WINDOW_WIDTH * 0.5f;
				y_[i] = (i / particles_per_row - particles_per_column / 2.0f + 0.5f) * spacey + WINDOW_HEIGHT * 0.5f;
				z_[i] = 0.0f;

				output_position_[i * 3 + 0] = x_[i];
				output_position_[i * 3 + 1] = y_[i];
				output_position_[i * 3 + 2] = z_[i];

				speedx_[i] = static_cast<float>(rand() % 1000 - 1000) / 100.0f;
				speedy_[i] = static_cast<float>(rand() % 1000 - 1000) / 100.0f;
				speedz_[i] = 0.0f;

				px_[i] = x_[i] - speedx_[i];
				py_[i] = y_[i] - speedy_[i];
				pz_[i] = z_[i] - speedz_[i];

				nx_[i] = x_[i];
				ny_[i] = y_[i];
				nz_[i] = z_[i];

				forcex_[i] = 0.0f;
				forcey_[i] = 0.0f;
				forcez_[i] = 0.0f;

				mass[i] = 1.0f;
				inv_mass[i] = 1.0f;

				radius_[i] = RADIUS;
			}
			break;
		}

		case DIMENSIONS::D3:
		{
			int cube_root = static_cast<int>(cbrt(BALL_NUMBER));

			float offset = 1.0f;

			float spacex = (RADIUS * 2.0f + offset);
			float spacey = (RADIUS * 2.0f + offset);
			float spacez = (RADIUS * 2.0f + offset);

			float startingx = RADIUS - 0.5f * (cube_root * 2.0f * RADIUS + (cube_root - 1) * offset);
			float startingy = RADIUS - 0.5f * (cube_root * 2.0f * RADIUS + (cube_root - 1) * offset);
			float startingz = RADIUS - 0.5f * (cube_root * 2.0f * RADIUS + (cube_root - 1) * offset);

			int x_index = 0;
			int y_index = 0;
			int z_index = 0;

			for (unsigned int i = 0; i < size_; i++)
			{
				id_[i] = i;

				x_[i] = startingx + (x_index % cube_root) * spacex;
				y_[i] = startingy + (y_index % cube_root) * spacey;
				z_[i] = startingz + (z_index % cube_root) * spacez;
				output_position_[i * 3 + 0] = x_[i];
				output_position_[i * 3 + 1] = y_[i];
				output_position_[i * 3 + 2] = z_[i];
				
				if (x_index < cube_root - 1)
				{
					x_index++;
				}
				else
				{
					x_index = 0;
					y_index++;
				}

				if (y_index == cube_root)
				{
					y_index = 0;
					z_index++;
				}

				speedx_[i] = static_cast<float>((rand() % 2000) - 1000) / 1000.0f;
				speedy_[i] = static_cast<float>((rand() % 2000) - 1000) / 1000.0f;
				speedz_[i] = static_cast<float>((rand() % 2000) - 1000) / 1000.0f;

				px_[i] = x_[i] - speedx_[i];
				py_[i] = y_[i] - speedy_[i];
				pz_[i] = z_[i] - speedz_[i];

				nx_[i] = x_[i];
				ny_[i] = y_[i];
				nz_[i] = z_[i];

				forcex_[i] = 0.0f;
				forcey_[i] = 0.0f;
				forcez_[i] = 0.0f;

				mass[i] = 1.0f;
				inv_mass[i] = 1.0f;

				radius_[i] = RADIUS;
			}
			break;
		}
	}
}
