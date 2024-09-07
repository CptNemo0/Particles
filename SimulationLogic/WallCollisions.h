#pragma once

#include "BallRepository.h"
#include "global.h"

class WallCollisions
{
	public:
		float top_;
		float down_;
		float left_;
		float right_;
		WallCollisions(float top, float down, float left, float right) : top_(top), down_(down), left_(left), right_(right) {};
		virtual void Init() = 0;
		virtual void CollideWalls(SOARepository& repository) = 0;
};

class LinearWallCollisions : public WallCollisions
{	
	public:
		LinearWallCollisions(float top, float down, float left, float right) : WallCollisions(top, down, left, right) {};
		void Init() override;
		void CollideWalls(SOARepository& repository) override;
};