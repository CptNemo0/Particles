#pragma once

#include "framework.h"

#include <memory>
#include "BallRepository.h"
#include "Mover.h"

class BallManager
{
private:
	BallManager(unsigned int size, Mover* mover);
	~BallManager() = default;
	
	Mover* mover_;

public:
	
	static BallManager* i_;
	SOARepository repository_;

	static void Initialize(unsigned int size, Mover* mover)
	{
		if (i_ == nullptr)
		{
			i_ = new BallManager(size, mover);
		}
	}

	static void Destroy()
	{
		if (i_ != nullptr)
		{
			i_ = nullptr;
			delete i_;
		}
	}

	void Step();
};