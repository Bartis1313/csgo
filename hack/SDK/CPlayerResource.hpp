#pragma once

#include <utility>

#include "helpers/netvars.hpp"

class PlayerResource
{
public:
	int getKills(int id)
	{
		const static auto addr = netvarMan.getNetvar("DT_PlayerResource", "m_iKills");
		return *reinterpret_cast<int*>(uintptr_t(this) + addr + id * sizeof(int*));
	}
	int getDeaths(int id)
	{
		const static auto addr = netvarMan.getNetvar("DT_PlayerResource", "m_iDeaths");
		return *reinterpret_cast<int*>(uintptr_t(this) + addr + id * sizeof(int*));
	}
	int getPing(int id)
	{
		const static auto addr = netvarMan.getNetvar("DT_PlayerResource", "m_iPing");
		return *reinterpret_cast<int*>(uintptr_t(this) + addr + id * sizeof(int*));
	}
	std::string getRank(int id, bool useShortName = false)
	{
		const static auto addr = netvarMan.getNetvar("DT_CSPlayerResource", "m_iCompetitiveRanking");

		const static std::pair<std::string, std::string> ranks[] =
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
		static size_t size = sizeof(ranks) / sizeof(ranks[0]);
		int rank = *reinterpret_cast<int*>(uintptr_t(this) + addr + id * sizeof(int*));
		const auto& str = !useShortName ? ranks[rank].first : ranks[rank].second;

		return str;
	}

	int getWins(int id)
	{
		const static auto addr = netvarMan.getNetvar("DT_CSPlayerResource", "m_iCompetitiveWins");
		return *reinterpret_cast<int*>(uintptr_t(this) + addr + id * sizeof(int*));
	}
};