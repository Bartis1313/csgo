#pragma once

class CUserCmd;

class CUserCmdCache
{
public:	
	static CUserCmd* getCmd() { return m_cache; }
	static void run(CUserCmd* cmd);
private:
	inline static CUserCmd* m_cache;
};