#pragma once
#include "../../SDK/math/Vector.hpp"
#include "../../SDK/math/Vector2D.hpp"
#include "../../SDK/math/matrix.hpp"

#include <numbers>
#include <span>
#include <vector>

struct ImVec2;

_NODISCARD constexpr float DEG2RAD(float degrees) { return degrees * (std::numbers::pi_v<float> / 180.0f); }
_NODISCARD constexpr float RAD2DEG(float radians) { return radians * (180.0f / std::numbers::pi_v<float>); }

// read: https://www.unknowncheats.me/forum/counterstrike-global-offensive/137492-math-behind-hack-1-coding-better-aimbot-stop-using-calcangle.html

namespace math
{
	constexpr float PI = std::numbers::pi_v<float>;
	_NODISCARD Vector calcAngleRelative(const Vector& src, const Vector& dest, const Vector& viewAngles);
	// classic
	_NODISCARD Vector calcAngle(const Vector& src, const Vector& dest);
	_NODISCARD float calcFov(const Vector& source, const Vector& destination, const Vector& viewAngles);
	// based on distance, difference is that we dont have same fov everytime, since this is dynamic	
	_NODISCARD float calcFovReal(const Vector& src, const Vector& dest, const Vector& viewAngles);
	_NODISCARD Vector transformVector(const Vector& in, const Matrix3x4& matrix);
	_NODISCARD Vector angleVec(const Vector& angle);
	_NODISCARD void angleVectors(const Vector& angle, Vector& forward, Vector& right, Vector& up);
	_NODISCARD float normalizeYaw(float yaw);
	_NODISCARD float normalizePitch(float pitch);
	// direction vector based on euler dimension
	_NODISCARD Vector vectorToAngle(const Vector& vec);
	_NODISCARD std::vector<ImVec2> grahamScan(std::span<const ImVec2> points);
	_NODISCARD std::vector<ImVec2> giftWrap(std::span<const ImVec2> points);
}