#pragma once

#include "Vector2D.hpp"

#include <cmath>
#include <algorithm>
#include <compare>

struct Vector
{
public:
	constexpr Vector()
		: x{ 0.0f }, y{ 0.0f }, z{ 0.0f }
	{}
	constexpr Vector(const float x, const float y, const float z)
		: x{ x }, y{ y }, z{ z }
	{}
	constexpr Vector(const float* arr)
		: x{ arr[0] }, y{ arr[1] }, z{ arr[2] }
	{}
	constexpr Vector(float* arr)
		: x{ arr[0] }, y{ arr[1] }, z{ arr[2] }
	{}

	inline static constexpr float MAX_ARG = 32767.0f;

	constexpr void clamp()
	{
		x = std::clamp(x, -89.0f, 89.0f);
		y = std::clamp(std::remainder(y, 360.0f), -180.0f, 180.0f);
		z = std::clamp(z, -50.0f, 50.0f);
	}

	[[nodiscard]] bool isValid() const
	{
		return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
	}

	[[nodiscard]] float length2D() const
	{
		return std::sqrt(x * x + y * y);
	}

	[[nodiscard]] constexpr float length2DSqrt() const
	{
		return (x * x + y * y);
	}

	[[nodiscard]] float length() const
	{
		return std::sqrt(x * x + y * y + z * z);
	}

	[[nodiscard]] float distToMeters(const Vector& vOther) const
	{
		return distTo(vOther) * 0.0254f;
	}

	[[nodiscard]] constexpr float lengthSqrt() const
	{
		return (x * x + y * y + z * z);
	}

	[[nodiscard]] constexpr bool isZero() const
	{
		return x == 0.0f && y == 0.0f && z == 0.0f;
	}

	[[nodiscard]] float distTo(const Vector& vOther) const
	{
		return (*this - vOther).length();
	}

	[[nodiscard]] constexpr Vector2D toVec2D() const
	{
		return Vector2D{ x, y };
	}

	Vector& normalize()
	{
		x = std::isfinite(x) ? std::remainder(x, 360.0f) : 0.0f;
		y = std::isfinite(y) ? std::remainder(y, 360.0f) : 0.0f;
		z = 0.0f;
		return *this;
	}

	[[nodiscard]] Vector normalized() const
	{
		Vector vec = *this;
		float len = vec.length();

		if (len)
			vec /= len;
		else
			vec = {};

		return vec;
	}

	[[nodiscard]] constexpr float dot(const Vector& v) const
	{
		return (x * v.x + y * v.y + z * v.z);
	}

	constexpr float& operator[](int i)
	{
		return ((float*)this)[i];
	}

	constexpr const float& operator[](int i) const
	{
		return ((float*)this)[i];
	}

	constexpr bool operator==(const Vector& src) const
	{
		return (src.x == x) && (src.y == y) && (src.z == z);
	}

	constexpr bool operator!=(const Vector& src) const
	{
		return (src.x != x) || (src.y != y) || (src.z != z);
	}

	constexpr auto operator<=>(const Vector&) const = default;

	constexpr Vector& operator=(const Vector& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	constexpr Vector& operator+=(const Vector& v)
	{
		x += v.x; y += v.y; z += v.z;
		return *this;
	}

	constexpr Vector& operator-=(const Vector& v)
	{
		x -= v.x; y -= v.y; z -= v.z;
		return *this;
	}

	constexpr Vector& operator*=(float fl)
	{
		x *= fl;
		y *= fl;
		z *= fl;
		return *this;
	}

	constexpr Vector& operator*=(const Vector& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	constexpr Vector& operator/=(const Vector& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	constexpr Vector& operator+=(float fl)
	{
		x += fl;
		y += fl;
		z += fl;
		return *this;
	}

	constexpr Vector& operator/=(float fl)
	{
		x /= fl;
		y /= fl;
		z /= fl;
		return *this;
	}

	constexpr Vector& operator-=(float fl)
	{
		x -= fl;
		y -= fl;
		z -= fl;
		return *this;
	}

	constexpr Vector operator+(const Vector& v) const
	{
		return Vector{ x + v.x, y + v.y, z + v.z };
	}

	constexpr Vector operator-(const Vector& v) const
	{
		return Vector{ x - v.x, y - v.y, z - v.z };
	}

	constexpr Vector operator*(const Vector& v) const
	{
		return Vector{ x * v.x, y * v.y, z * v.z };
	}

	constexpr Vector operator/(const Vector& v) const
	{
		return Vector{ x / v.x, y / v.y, z / v.z };
	}

	constexpr Vector operator*(float fl) const
	{
		return Vector{ x * fl, y * fl, z * fl };
	}

	constexpr Vector operator/(float fl) const
	{
		return Vector{ x / fl, y / fl, z / fl };
	}

	float x, y, z;
};

class __declspec(align(16)) VectorAligned : public Vector
{
public:
	VectorAligned() = default;

	constexpr VectorAligned(const Vector& v)
		: Vector{ v }, w{ 0.0f }
	{}

	constexpr VectorAligned& operator=(const Vector& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = 0.0f;
		return *this;
	}

public:
	float w;
};