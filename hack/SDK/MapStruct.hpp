#pragma once

#include "math/Vector.hpp"
#include "helpers/pad.hpp"

struct MapStruct
{
	PAD(276);
	Vec2 m_origin;
	PAD(12);
	float m_scale;
	PAD(200);
	int m_numberRadarPlayers;
};