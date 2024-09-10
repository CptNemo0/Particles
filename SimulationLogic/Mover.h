#pragma once

#include "framework.h"

#include "BallRepository.h"

class Mover
{
	public:
		Mover() = default;
		~Mover() = default;

		virtual void Init      (const SOARepository& repository) = 0;
		virtual void UpdateVelocities(SOARepository& repository) = 0;
		virtual void PredictPositions(SOARepository& repository) = 0;
		virtual void UpdatePositions (SOARepository& repository) = 0;
};

class NormalMover : public Mover
{
	public:
		NormalMover();
		~NormalMover() = default;

		void Init	   (const SOARepository& repository) override;
		void UpdateVelocities(SOARepository& repository) override;
		void PredictPositions(SOARepository& repository) override;
		void UpdatePositions (SOARepository& repository) override;
};

class SIMDMover : public Mover
{
	public:
		SIMDMover();
		~SIMDMover() = default;
		
		void Init      (const SOARepository& repository) override;
		void UpdateVelocities(SOARepository& repository) override;
		void PredictPositions(SOARepository& repository) override;
		void UpdatePositions (SOARepository& repository) override;
};