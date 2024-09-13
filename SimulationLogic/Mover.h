#pragma once

#include "framework.h"
#include "BallRepository.h"

class Mover
{
	protected:
		SOARepository* repository_;
	public:
		Mover(SOARepository* repository) : repository_(repository) {};
		~Mover() = default;

		virtual void Init		     () = 0;
		virtual void UpdateVelocities() = 0;
		virtual void PredictPositions() = 0;
		virtual void AdjustVelocities() = 0;
		virtual void UpdatePositions () = 0;
};

class NormalMover : public Mover
{
	public:
		NormalMover(SOARepository* repository) : Mover(repository) {};
		~NormalMover() = default;

		void Init		     () override;
		void UpdateVelocities() override;
		void PredictPositions() override;
		void AdjustVelocities() override;
		void UpdatePositions () override;
};

class SIMDMover : public Mover
{
	public:
		SIMDMover(SOARepository* repository) : Mover(repository) {};
		~SIMDMover() = default;
		
		void Init		     () override;
		void UpdateVelocities() override;
		void PredictPositions() override;
		void UpdatePositions () override;
		void AdjustVelocities() ;
};