#pragma once

#include <SDK/math/matrix.hpp>
#include <SDK/math/Vector.hpp>

#include <numbers>
#include <span>
#include <vector>
#include <utility>
#include <tuple>
#include <optional>

struct ImVec2;

// read: https://www.unknowncheats.me/forum/counterstrike-global-offensive/137492-math-behind-hack-1-coding-better-aimbot-stop-using-calcangle.html
namespace math
{
	constexpr static float PI = std::numbers::pi_v<float>;
	constexpr static float PI_2 = std::numbers::pi_v<float> * 2.0f;

	[[nodiscard]] constexpr static float DEG2RAD(float degrees) { return degrees * (PI / 180.0f); }
	[[nodiscard]] constexpr static float RAD2DEG(float radians) { return radians * (180.0f / PI); }

	[[nodiscard]] Vec3 calcAngleRelative(const Vec3& src, const Vec3& dest, const Vec3& viewAngles);
	// classic
	[[nodiscard]] Vec3 calcAngle(const Vec3& src, const Vec3& dest);
	[[nodiscard]] float calcFov(const Vec3& source, const Vec3& destination, const Vec3& viewAngles);
	// based on distance, difference is that we dont have same fov everytime, since this is dynamic	
	[[nodiscard]] float calcFovReal(const Vec3& src, const Vec3& dest, const Vec3& viewAngles);
	[[nodiscard]] Vec3 transformVector(const Vec3& in, const Matrix3x4& matrix);
	[[nodiscard]] Vec3 angleVec(const Vec3& angle);
	[[nodiscard]] std::tuple<Vec3, Vec3, Vec3> angleVectors(const Vec3& angle);
	[[nodiscard]] float normalizeYaw(float yaw);
	[[nodiscard]] float normalizePitch(float pitch);
	// direction vector based on euler dimension
	[[nodiscard]] Vec3 vectorToAngle(const Vec3& vec);
	[[nodiscard]] std::optional<std::vector<ImVec2>> grahamScan(std::span<const ImVec2> points);
	[[nodiscard]] std::optional<std::vector<ImVec2>> giftWrap(std::span<const ImVec2> points);
	[[nodiscard]] std::pair<Vec3, Vec3> transformAABB(const Matrix3x4& transform, const Vec3& mins, const Vec3& maxs);
}