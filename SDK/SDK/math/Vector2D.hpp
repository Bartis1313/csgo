#pragma once

#include <compare>

struct Vector2D
{
	constexpr Vector2D()
		: x{ 0.0f }, y{ 0.0f }
	{}

	constexpr Vector2D(float x, float y)
		: x{ x }, y{ y }
	{}

	constexpr auto operator<=>(const Vector2D&) const = default;

	constexpr Vector2D& operator+=(const Vector2D& v)
	{
		x += v.x; y += v.y;
		return *this;
	}

	constexpr Vector2D& operator-=(const Vector2D& v)
	{
		x -= v.x; y -= v.y;
		return *this;
	}

	constexpr Vector2D operator+(const Vector2D& v) const
	{
		return Vector2D{ x + v.x, y + v.y };
	}

	constexpr Vector2D operator-(const Vector2D& v) const
	{
		return Vector2D{ x - v.x, y - v.y };
	}

	[[nodiscard]] float length() const
	{
		return std::sqrt(x * x + y * y);
	}

	[[nodiscard]] float distTo(const Vector2D& vOther) const
	{
		return (*this - vOther).length();
	}

	[[nodiscard]] constexpr bool isZero() const
	{
		return x == 0.0f && y == 0.0f;
	}

	float x;
	float y;
};