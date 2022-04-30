#pragma once

#include "math/Vector.hpp"
#include "Color.hpp"

class CTeslaInfo
{
public:
	Vector m_pos;
	Vector m_angles;
	int	m_entIndex;
	const char* m_spriteName;
	float m_beamWidth;
	int m_beams;
	Vector m_color;
	float m_timeVisible;
	float m_radius;

	void setColor(const Color& color)
	{
		m_color = { color.r(), color.g(), color.b() };
	}
};
