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
		float near_;
		float far_;
		WallCollisions(float top, float down, float left, float right) : top_(top), down_(down), left_(left), right_(right), near_(0.0f), far_(0.0f){};
		WallCollisions(float top, float down, float left, float right, float _far, float _near) : top_(top), down_(down), left_(left), right_(right), far_(_far), near_(_near) {};
		virtual void Init() = 0;
		virtual void CollideWalls(SOARepository& repository) = 0;
};

class WallCollisions2D : public WallCollisions
{	
	public:
		WallCollisions2D(float top, float down, float left, float right) : WallCollisions(top, down, left, right) {};
		void Init() override;
		void CollideWalls(SOARepository& repository) override;
};

class WallCollisions3D : public WallCollisions
{
public:
	WallCollisions3D(float top, float down, float left, float right, float _far, float _near) : WallCollisions(top, down, left, right, _far, _near) {};
	void Init() override;
	void CollideWalls(SOARepository& repository) override;
};