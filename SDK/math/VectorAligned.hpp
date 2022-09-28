#pragma once

#include "Vector.hpp"

#include <ranges>

class VectorAligned : public Vec3
{
public:
	constexpr VectorAligned() = default;

	constexpr VectorAligned(const Vec3& v) :
		Vec3{ v }, w{ 0.0f }
	{}

	constexpr VectorAligned& operator=(const Vec3& v)
	{
		for (auto i : std::views::iota(0U, v.size()))
			m_arr[i] = v[i];

		w = 0.0f;
		return *this;
	}

	float w = {};
};