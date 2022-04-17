#pragma once

#include <utility>

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
	NETVAR_BY_INDEX(int, getWins, m_iCompetitiveWins);
    std::string getRank(int idx, bool useShortName = false)
    {
        static std::pair<std::string, std::string> ranks[] =
        {
            { "-", "-"},
            {"Silver I", "S1"},
            {"Silver II", "S2"},
            {"Silver III", "S3"},
            {"Silver IV", "S4"},
            {"Silver Elite", "SE"},
            {"Silver Elite Master", "SEM"},
            {"Gold Nova I", "G1"},
            {"Gold Nova II", "G2"},
            {"Gold Nova III", "G3"},
            {"Gold Nova Master", "GM"},
            {"Master Guardian I", "MG1"},
            {"Master Guardian II", "MG2"},
            {"Master Guardian Elite", "MGE"},
            {"Distinguished Master Guardian", "DMG"},
            {"Legendary Eagle", "LE"},
            {"Legendary Eagle Master", "LEM"},
            {"Supreme Master First Class", "SUPREME"},
            {"The Global Elite", "GLOBAL"}
        };

        auto str = !useShortName ? ranks[m_getRank(idx)].first : ranks[m_getRank(idx)].second;

        return str;
    }
private: // 65 is supposed to be max players, correct if this is higher, who plays on such big servers though?
	NETVAR(int[65], m_iKills, "DT_PlayerResource", "m_iKills");
	NETVAR(int[65], m_iDeaths, "DT_PlayerResource", "m_iDeaths");
	NETVAR(int[65], m_iPing, "DT_PlayerResource", "m_iPing");
	NETVAR(int[65], m_iCompetitiveRanking, "DT_PlayerResource", "m_iCompetitiveRanking");
	NETVAR(int[65], m_iCompetitiveWins, "DT_PlayerResource", "m_iCompetitiveWins");
	NETVAR_BY_INDEX(int, m_getRank, m_iCompetitiveRanking);
};