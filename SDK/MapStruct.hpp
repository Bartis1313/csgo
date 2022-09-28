#pragma once

#include "math/Vector.hpp"
#include <utilities/pad.hpp>

struct MapStruct
{
	PAD(256);
	Vec2 m_origin;
	PAD(12);
	float m_scale;
	PAD(800);
};