#pragma once
#include <immintrin.h>
#include "framework.h"

namespace 
{
	const unsigned int MAX_BALL_NUMBER = 50000;
	const unsigned int BALL_NUMBER = 1024 * 15; //*40

	constexpr float FIXED_DT = 1.0f / 180.0f;
	constexpr float INV_FIXED_DT = 1.0f / FIXED_DT;

	unsigned int SEED = 9082024;

	const char* WINDOW_TITLE = "Particles";

	const int WINDOW_WIDTH = 1920;
	const int WINDOW_HEIGHT = 1080;

	const float RADIUS = 0.5f;    //1.5f
	constexpr float INFLUENCE_RADIUS = 1.0f;
	const float CELL_SIZE = 1.0;
	
	const float MAX_SPEED = 100.0f;

	const  int SIMD_BLOCK_SIZE = 8;

	const int SOLVER_ITERATIONS = 1;
	constexpr float REST_DENSITY = 1.0f;
	constexpr float INV_REST_DENSITY = 1.0f / REST_DENSITY;
	constexpr float RELAXATION_PARAMETER = 100.0f;
}