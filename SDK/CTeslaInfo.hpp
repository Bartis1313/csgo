#pragma once

#include "math/Vector.hpp"
#include "Color.hpp"

class CTeslaInfo
{
public:
	Vec3 m_pos;
	Vec3 m_angles;
	int m_entIndex;
	const char* m_spriteName;
	float m_beamWidth;
	int m_beams;
	Vec3 m_color;
	float m_timeVisible;
	float m_radius;

	void setColor(const Color& color)
	{
		m_color = Vec3{ color.r(), color.g(), color.b() };
	}
};
