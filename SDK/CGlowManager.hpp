#pragma once
#include "Color.hpp"
#include "math/Vector.hpp"
#include "../utilities/pad.hpp"

class CGlowObject_t
{
public:
	void set(const Color& color)
	{
		rgb = Vector(color.r(), color.g(), color.b());
		a = color.a();
		m_occluded = true;
		m_unoccluded = false;
		m_bloomAmount = 1.0f;
	}
	bool unused()
	{
		return m_nextFree != -2;
	}

	int m_nextFree;
	void* m_entity;
	Vector rgb;
	float a;
	PAD(8);
	float m_bloomAmount;
	PAD(4);
	bool m_occluded;
	bool m_unoccluded;
	bool m_fullBloom;
	PAD(13);
};

class CGlowManager
{
public:
	CGlowObject_t* m_objects;
	PAD(8);
	int m_size;
};