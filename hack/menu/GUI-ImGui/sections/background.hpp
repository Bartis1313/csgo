#pragma once

#include <render/Color.hpp>
#include <SDK/math/Vector.hpp>

struct ParticlePoint_t
{
	Vec2 m_pos{ }; // actual pos to draw
	Vec2 m_move{ }; // how much to move the element +/- need small values!
	Color m_color{ };
	float m_alpha{ 1.0f };
};

namespace background
{
	void init();
	void draw();
};