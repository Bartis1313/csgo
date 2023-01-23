#pragma once

#include "Vector.hpp"

#include <array>

template<size_t ROWS = 3, size_t COLS = 4>
struct Matrix
{
	constexpr Matrix() = default;

	[[nodiscard]] constexpr auto origin() const
	{
		return Vec3{ m_matValues[0][3], m_matValues[1][3], m_matValues[2][3] };
	}

	constexpr const auto operator[](size_t i) const
	{
		return m_matValues[i];
	}

	constexpr auto operator[](size_t i)
	{
		return m_matValues[i];
	}
private:
	std::array<std::array<float, COLS>, ROWS> m_matValues;

	static_assert(ROWS >= 2 && ROWS >= 2, "Did you want to create a vector?");
};

using Matrix3x4 = Matrix<3, 4>;
using Matrix4x4 = Matrix<4, 4>;