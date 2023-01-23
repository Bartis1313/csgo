#pragma once

#include <render/Color.hpp>
#include "helpers/pad.hpp"

class CGlowObject_t
{
public:
	void set(const Color& color, bool occluded = true, bool unocculuded = false, bool fullBloom = false, float amount = 1.0f)
	{
		m_color = color;
		m_occluded = occluded;
		m_unoccluded = unocculuded;
		m_fullBloom = fullBloom;
		m_bloomAmount = amount;
	}
	bool unused()
	{
		return m_nextFree != -2;
	}

	int m_nextFree;
	void* m_entity;
	Color m_color;
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