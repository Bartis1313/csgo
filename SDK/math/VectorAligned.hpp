#pragma once

#include "Vector.hpp"

struct VectorAligned : public Vec3
{
public:
	VectorAligned() = default;

	constexpr VectorAligned(const Vec3& v) :
		w{ 0.0f }
	{
		m_arr[0] = v[0];
		m_arr[1] = v[1];
		m_arr[2] = v[2];
		w = 0.0f;
	}

	constexpr VectorAligned& operator=(const Vec3& v)
	{
		m_arr[0] = v[0];
		m_arr[1] = v[1];
		m_arr[2] = v[2];
		w = 0.0f;

		return *this;
	}

	float w = 0.0f;
};