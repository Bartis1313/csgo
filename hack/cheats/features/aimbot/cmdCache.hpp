#pragma once

#include <optional>

class CUserCmd;
class CfgWeapon;

class CUserCmdCache
{
public:	
	static CUserCmd* getCmd() { return m_cache; }
	static std::optional<CfgWeapon> getWeaponConfig();
	static void run(CUserCmd* cmd);
private:
	inline static CUserCmd* m_cache;
};