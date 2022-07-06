#pragma once

#include "../utilities/pad.hpp"
#include "../SDK/math/Vector2D.hpp"

struct MapStruct
{
	PAD(256);
	Vector2D m_origin;
	PAD(12);
	float m_scale;
	PAD(800);
};