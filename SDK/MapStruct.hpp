#pragma once

#include "math/Vector2D.hpp"
#include <utilities/pad.hpp>

struct MapStruct
{
	PAD(256);
	Vector2D m_origin;
	PAD(12);
	float m_scale;
	PAD(800);
};