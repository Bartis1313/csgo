#pragma once
#include "../../SDK/math/Vector.hpp"
#include "../../SDK/math/Vector2D.hpp"
#include "../../SDK/math/matrix.hpp"
#include <numbers>

constexpr auto DEG2RAD(float degrees) { return degrees * (std::numbers::pi_v<float> / 180.0f); }
constexpr auto RAD2DEG(float radians) { return radians * (180.0f / std::numbers::pi_v<float>); }

// read: https://www.unknowncheats.me/forum/counterstrike-global-offensive/137492-math-behind-hack-1-coding-better-aimbot-stop-using-calcangle.html

namespace math
{
	_NODISCARD Vector calcAngleRelative(const Vector& src, const Vector& dest, const Vector& viewAngle);
	float calcFov(const Vector& source, const Vector& destination, const Vector& viewAngles);	
	_NODISCARD Vector transformVector(const Vector& in, const matrix3x4_t& matrix);
	_NODISCARD Vector angleVec(const Vector& angle);
	_NODISCARD float normalizeYaw(float yaw);
	// direction vector based on euler dimension
	_NODISCARD Vector vectorToAngle(const Vector& vec);
}