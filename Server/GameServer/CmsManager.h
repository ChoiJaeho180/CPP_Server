#pragma once
#include "CmsCached.h"

class CmsManager
{
public:
	static CmsManager& GetInstance()
	{
		static CmsManager instance;
		return instance;
	}

	void Init(const string& directory);

	template<typename T>
	const T* Get(int id) const { return CmsCached<T>::Get(id); }
};

