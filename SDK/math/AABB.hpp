#pragma once

#include "Vector.hpp"

struct AABB_t
{
	Vector<float>::v3 m_minBounds;
	Vector<float>::v3 m_maxBounds;
};