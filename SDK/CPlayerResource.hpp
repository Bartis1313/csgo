#pragma once
#include "../utilities/netvars/netvars.hpp"

// not really a netvar return address, but return valid value
// type - template type for return type
// name - name your function
// netvarFun - passed by name function that returns correct address to that netvar, NOT VALUE
#define NETVAR_BY_INDEX(type, name, netvarFun) \
_NODISCARD type name(const int index) { \
	return netvarFun()[index]; \
    }

class PlayerResource
{
public:
	NETVAR_BY_INDEX(int, getKills, m_iKills);
	NETVAR_BY_INDEX(int, getDeaths, m_iDeaths);
	NETVAR_BY_INDEX(int, getPing, m_iPing);
private: // 65 is supposed to be max players, correct if this is higher, who plays on such big servers though?
	NETVAR(int[65], m_iKills, "DT_PlayerResource", "m_iKills");
	NETVAR(int[65], m_iDeaths, "DT_PlayerResource", "m_iDeaths");
	NETVAR(int[65], m_iPing, "DT_PlayerResource", "m_iPing");
};