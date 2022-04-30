#pragma once
#include <compare>

struct Vector2D
{
	constexpr Vector2D()
		: x{ 0.0f }, y{ 0.0f }
	{}
	// template due to making it working like pair
	template<typename T = float>
	constexpr Vector2D(const T t1, const T t2)
		: x{ static_cast<float>(t1) }, y{ static_cast<float>(t2) } // casting due to making it always as a float no matter what
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

	_NODISCARD float length() const
	{
		return std::sqrt(x * x + y * y);
	}

	_NODISCARD float distTo(const Vector2D& vOther) const
	{
		return (*this - vOther).length();
	}

	_NODISCARD constexpr bool isZero() const
	{
		return x == 0.0f && y == 0.0f;
	}

	float x;
	float y;
};