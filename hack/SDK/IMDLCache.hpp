#pragma once

#include "helpers/vfunc.hpp"

class IMDLCache
{
public:
	VFUNC(void, beginLock, 33, (), (this));
	VFUNC(void, endLock, 34, (), (this));
};

class CMDLCacheCriticalSection
{
public:
	CMDLCacheCriticalSection(IMDLCache* cache)
		: cache{ cache }
	{
		this->cache->beginLock();
	}

	~CMDLCacheCriticalSection()
	{
		cache->endLock();
	}
private:
	IMDLCache* cache;
};

#define MDLCACHE_CRITICAL_SECTION(cache) CMDLCacheCriticalSection cacheCriticalSection(cache)