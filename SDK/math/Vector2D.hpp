#pragma once

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

	[[nodiscard]] constexpr bool isZero() const
	{
		return x == 0.0f && y == 0.0f;
	}

	float x;
	float y;
};