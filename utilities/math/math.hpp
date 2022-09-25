#pragma once

#include <SDK/math/matrix.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/math/Vector2D.hpp>

#include <numbers>
#include <span>
#include <vector>
#include <tuple>

struct ImVec2;

// read: https://www.unknowncheats.me/forum/counterstrike-global-offensive/137492-math-behind-hack-1-coding-better-aimbot-stop-using-calcangle.html
namespace math
{
	constexpr static float PI = std::numbers::pi_v<float>;
	constexpr static float PI_2 = std::numbers::pi_v<float> * 2.0f;

	_NODISCARD constexpr static float DEG2RAD(float degrees) { return degrees * (PI / 180.0f); }
	_NODISCARD constexpr static float RAD2DEG(float radians) { return radians * (180.0f / PI); }

	_NODISCARD Vector calcAngleRelative(const Vector& src, const Vector& dest, const Vector& viewAngles);
	// classic
	_NODISCARD Vector calcAngle(const Vector& src, const Vector& dest);
	_NODISCARD float calcFov(const Vector& source, const Vector& destination, const Vector& viewAngles);
	// based on distance, difference is that we dont have same fov everytime, since this is dynamic	
	_NODISCARD float calcFovReal(const Vector& src, const Vector& dest, const Vector& viewAngles);
	_NODISCARD Vector transformVector(const Vector& in, const Matrix3x4& matrix);
	_NODISCARD Vector angleVec(const Vector& angle);
	_NODISCARD std::tuple<Vector, Vector, Vector> angleVectors(const Vector& angle);
	_NODISCARD float normalizeYaw(float yaw);
	_NODISCARD float normalizePitch(float pitch);
	// direction vector based on euler dimension
	_NODISCARD Vector vectorToAngle(const Vector& vec);
	_NODISCARD std::vector<ImVec2> grahamScan(std::span<const ImVec2> points);
	_NODISCARD std::vector<ImVec2> giftWrap(std::span<const ImVec2> points);
	_NODISCARD std::pair<Vector, Vector> transformAABB(const Matrix3x4& transform, const Vector& mins, const Vector& maxs);
}