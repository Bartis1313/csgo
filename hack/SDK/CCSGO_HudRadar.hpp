#pragma once

#include "math/Vector.hpp"
#include "helpers/pad.hpp"

// client.dll 55 8B EC 83 E4 F8 83 EC 60 A1 ? ? ? ? 56 57 | client.dll 55 8B EC 83 EC 18 8B 45 08 53 56 57
// call in ProcessSpottedEntityUpdate as game originally do
// this is why the fields might give not always pure 1:1 results to frequently updated player related fields
struct RadarEntity
{
	Vec3 m_origin;
	Vec3 m_viewAngles;
	PAD(28);
	float m_spottedTime;
	PAD(16);
	int m_index;
	PAD(4);
	int m_health;
	char m_name[32];
	PAD(117);
	bool m_visible;
	PAD(138);
	/*void write() const
	{
		printf("origin iszero? %i index %i health %i name %s visible %i\n", m_origin.isZero(), m_index, m_health, m_name, m_visible);
	}*/
};
static_assert(sizeof(RadarEntity) == 372);

struct CSRadar
{
	PAD(332);
	RadarEntity m_players[65];
};

struct CCSGO_HudRadar
{
	PAD(12);
	int m_maybeSizeOfImageRadar;
	PAD(512);
	int m_sizeOfPlayers;
};