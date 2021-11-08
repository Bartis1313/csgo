#pragma once
#include "../utilities/netvars/netvars.hpp"

class PlayerResource
{
public:
	int getKills(int id)
	{
		static auto m_iKills = NetvarManager::g().getNetvar(XOR("DT_PlayerResource"), XOR("m_iKills"));
		return *reinterpret_cast<int*>((uintptr_t)this + m_iKills + id * 0x4);
	}

	int getDeaths(int id)
	{
		const static auto m_iDeaths = NetvarManager::g().getNetvar(XOR("DT_PlayerResource"), XOR("m_iDeaths"));
		return *reinterpret_cast<int*>((uintptr_t)this + m_iDeaths + id * 0x4);
	}

	int getPing(int id)
	{
		const static auto m_iPing = NetvarManager::g().getNetvar(XOR("DT_PlayerResource"), XOR("m_iPing"));
		return *reinterpret_cast<int*>((uintptr_t)this + m_iPing + id * 0x4);
	}
};