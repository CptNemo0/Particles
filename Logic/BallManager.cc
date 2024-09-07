#include "BallManager.h"
#include "pch.h"
#include <iostream>

BallManager* BallManager::i_ = nullptr;

BallManager::BallManager(unsigned int size, Mover* mover)
{
    repository_ = SOARepository(size);
    mover_ = mover;
    mover_->Init(repository_);
}

void BallManager::Step()
{
    mover_->Step(repository_);
}

