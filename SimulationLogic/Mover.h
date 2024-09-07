#pragma once

#include "framework.h"

#include "BallRepository.h"

class Mover
{
	public:
		Mover() = default;
		~Mover() = default;

		virtual void Init(const SOARepository& repository) = 0;
		virtual void Step(SOARepository& repository) = 0;
};

class NormalMover : public Mover
{
	public:
		NormalMover();
		~NormalMover() = default;

		void Step(SOARepository& repository) override;
		void Init(const SOARepository& repository) override;
		void PredictPositions(SOARepository& repository);
		void UpdatePositions(SOARepository& repository);
		void UpdateVelocities(SOARepository& repository);
};

class SIMDMover : public Mover
{
	public:
		SIMDMover();
		~SIMDMover() = default;
		
		void Init(const SOARepository& repository) override;
		void Step(SOARepository& repository) override;
		void PredictPositions(SOARepository& repository);
		void UpdatePositions(SOARepository& repository);
};