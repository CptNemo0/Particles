#pragma once
#include <immintrin.h>
#include "framework.h"

namespace 
{
	const unsigned int MAX_BALL_NUMBER = 50000;
	const unsigned int BALL_NUMBER = 1024*25; //*40

	const float FIXED_DT = 1.0f / 240.0f;

	unsigned int SEED = 1092024;

	const char* WINDOW_TITLE = "Particles";

	const int WINDOW_WIDTH = 1600;
	const int WINDOW_HEIGHT = 900;

	const float RADIUS = 0.5f;    //1.5f
	const float INFLUENCE_RADIUS = 1.0f;
	const float CELL_SIZE = 1.0f;
	
	const float MAX_SPEED = 100.0f;

	const  int SIMD_BLOCK_SIZE = 8;
}