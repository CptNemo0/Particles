#pragma once

#include "BallRepository.h"
#include "global.h"

class WallCollisions
{
	protected:
		SOARepository* repository_;
	public:
		float top_;
		float down_;
		float left_;
		float right_;
		float near_;
		float far_;
		WallCollisions(float top, float down, float left, float right, SOARepository* repository) : top_(top), down_(down), left_(left), right_(right), near_(0.0f), far_(0.0f), repository_(repository){};
		WallCollisions(float top, float down, float left, float right, float _far, float _near, SOARepository* repository) : top_(top), down_(down), left_(left), right_(right), far_(_far), near_(_near), repository_(repository){};
		virtual void Init() = 0;
		virtual void CollideWalls() = 0;
};

class WallCollisions2D : public WallCollisions
{	
	public:
		WallCollisions2D(float top, float down, float left, float right, SOARepository* repository) : WallCollisions(top, down, left, right, repository) {};
		void Init() override;
		void CollideWalls() override;
};

class WallCollisions3D : public WallCollisions
{
public:
	WallCollisions3D(float top, float down, float left, float right, float _far, float _near, SOARepository* repository) : WallCollisions(top, down, left, right, _far, _near, repository) {};
	void Init() override;
	void CollideWalls() override;
};