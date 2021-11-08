#pragma once
#include "Color.hpp"
#include "../utilities/pad.hpp"
#include "math/Vector.hpp"

// don't input color class to the struct, here they use 4 bytes for each rgba

class CGlowObject_t
{
public:
	void set(Color color)
	{
		rgb = Vector(color.rDevided(), color.gDevided(), color.bDevided());
		a = color.aDevided();
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