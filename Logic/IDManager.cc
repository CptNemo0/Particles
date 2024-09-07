#include "IDManager.h"
#include "pch.h"

IDManager* IDManager::i_ = nullptr;

IDManager::IDManager()
{
	id_counter_ = 0;
}