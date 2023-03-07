#include "cmdCache.hpp"

void CUserCmdCache::run(CUserCmd* cmd)
{
	m_cache = cmd;
}