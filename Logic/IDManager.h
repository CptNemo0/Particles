#pragma once

#include "framework.h"

const unsigned int EMPTY_ID = 4294967294;

class IDManager
{
	private:
		IDManager();
		~IDManager() = default;

		unsigned int id_counter_;

	public:
		static IDManager* i_;

		static void Initialize()
		{
			if (i_ == nullptr)
			{
				i_ = new IDManager();
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

		unsigned int GetId()
		{
			return id_counter_++;
		}
};

