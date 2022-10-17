#include "math.hpp"

Vec3 math::calcAngleRelative(const Vec3& source, const Vec3& destination, const Vec3& viewAngles)
{
	const auto delta = destination - source;

	Vec3 angles
	{
		RAD2DEG(std::atan2(-delta[2], std::hypot(delta[0], delta[1]))) - viewAngles[0],
		RAD2DEG(std::atan2(delta[1], delta[0])) - viewAngles[1],
		0.0f
	};

	return angles.normalize();
}

Vec3 math::calcAngle(const Vec3& source, const Vec3& destination)
{
	const auto delta = destination - source;

	Vec3 angles
	{
		RAD2DEG(std::atan2(-delta[2], std::hypot(delta[0], delta[1]))),
		RAD2DEG(std::atan2(delta[1], delta[0])),
		0.0f
	};

	return angles.normalize();
}

float math::calcFov(const Vec3& source, const Vec3& destination, const Vec3& viewAngles)
{
	const auto angle = calcAngleRelative(source, destination, viewAngles);
	const auto fov = std::hypot(angle[0], angle[1]);
	return fov;
}

float math::calcFovReal(const Vec3& source, const Vec3& destination, const Vec3& viewAngles)
{
	float dist = source.distTo(destination);
	float realDist = dist / 10.0f;

	auto start = angleVec(viewAngles) * realDist;
	auto end = angleVec(calcAngle(source, destination)) * realDist;

	return start.distTo(end);
}

Vec3 math::transformVector(const Vec3& in, const Matrix3x4& matrix)
{
	return Vec3(in.dot(Vec3{ matrix[0].data() }) + matrix[0][3], in.dot(Vec3{ matrix[1].data() }) + matrix[1][3],
		in.dot(Vec3{ matrix[2].data() }) + matrix[2][3]);
}

Vec3 math::angleVec(const Vec3& angle)
{
	auto sy = std::sin(DEG2RAD(angle[1]));
	auto cy = std::cos(DEG2RAD(angle[1]));

	auto sp = std::sin(DEG2RAD(angle[0]));
	auto cp = std::cos(DEG2RAD(angle[0]));

	return Vec3{ cp * cy, cp * sy, -sp };
}

std::tuple<Vec3, Vec3, Vec3> math::angleVectors(const Vec3& angle)
{
	auto sy = std::sin(DEG2RAD(angle[1]));
	auto cy = std::cos(DEG2RAD(angle[1]));

	auto sp = std::sin(DEG2RAD(angle[0]));
	auto cp = std::cos(DEG2RAD(angle[0]));

	auto sr = std::sin(DEG2RAD(angle[2]));
	auto cr = std::cos(DEG2RAD(angle[2]));

	Vec3 forward
	{
		cp * cy,
		cp * sy,
		-sp
	};

	Vec3 right
	{
		-1.0f * sr * sp * cy + -1.0f * cr * -sy,
		-1.0f * sr * sp * sy + -1.0f * cr * cy,
		-1.0f * sr * cp
	};	

	Vec3 up
	{
		cr * sp * cy + -sr * -sy,
		cr * sp * sy + -sr * cy,
		cr * cp
	};

	return { forward, right, up };
}

float math::normalizeYaw(float yaw)
{
	while (yaw > 180.0f)
		yaw -= 360.0f;

	while (yaw < -180.0f)
		yaw += 360.0f;

	return yaw;
}

float math::normalizePitch(float pitch)
{
	while (pitch > 89.0f)
		pitch -= 180.0f;

	while (pitch < -89.0f)
		pitch += 180.0f;

	return pitch;
}

Vec3 math::vectorToAngle(const Vec3& vec)
{
	Vec3 angle;
	if (vec[0] == 0.0f && vec[1] == 0.0f)
	{
		angle[0] = (vec[2] > 0.0f) ? 270.0f : 90.0f;
		angle[1] = 0.0f;
	}
	else
	{
		angle[0] = RAD2DEG(std::atan2(-vec[2], std::hypot(vec[0], vec[1])));
		angle[1] = RAD2DEG(std::atan2(vec[1], vec[0]));

		if (angle[1] > 90.0f)
			angle[1] -= 180.0f;
		else if (angle[1] < 90.0f)
			angle[1] += 180.0f;
		else if (angle[1] == 90.0f)
			angle[1] = 0.0f;
	}
	angle[2] = 0.0f;
	return angle;
}

std::pair<Vec3, Vec3> math::transformAABB(const Matrix3x4& transform, const Vec3& mins, const Vec3& maxs)
{
	Vec3 localCenter = mins + maxs;
	localCenter *= 0.5f;

	Vec3 localExtents = maxs - localCenter;

	Vec3 worldCenter = transformVector(localCenter, transform);

	Vec3 worldExtents;

	worldExtents[0] = localExtents.dot(Vec3{ transform[0].data() });
	worldExtents[1] = localExtents.dot(Vec3{ transform[1].data() });
	worldExtents[2] = localExtents.dot(Vec3{ transform[2].data() });

	return {
		worldCenter - worldExtents,
		worldCenter + worldExtents
	};
}

#include <dependencies/ImGui/imgui_impl_dx9.h>
#include <algorithm>

static constexpr float orient(const ImVec2& a, const ImVec2& b, const ImVec2& c)
{
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

std::optional<std::vector<ImVec2>> math::grahamScan(std::span<const ImVec2> points)
{
	// case when it's impossible
	if (points.size() < 3)
		std::nullopt;

	// make a temp copy to allow use swap
	std::vector<ImVec2> v{ points.begin(), points.end() };

	// now at index 0 we have most left point
	std::swap(v.at(0), *std::min_element(v.begin(), v.end(),
		// is left? a lexicographically before b
		[](const ImVec2& a, const ImVec2& b) constexpr
		{
			return (a.x < b.x || (a.x == b.x && a.y < b.y));
		}
	));

	std::sort(v.begin() + 1, v.end(),
		[p0 = v.at(0)](const ImVec2& a, const ImVec2& b) constexpr
		{
			auto _orient = orient(p0, a, b);
			return _orient == 0.0f ? ImLengthSqr(p0 - a) < ImLengthSqr(p0 - b) : _orient < 0.0f;
		}
	);

	std::vector<ImVec2> hull;

	for (const auto& el : v)
	{
		while (hull.size() > 1 && orient(hull[hull.size() - 2], hull[hull.size() - 1], el) >= 0.0f)
			hull.pop_back();

		hull.push_back(el);
	}

	return hull;
}

std::optional<std::vector<ImVec2>> math::giftWrap(std::span<const ImVec2> points)
{
	// case when it's impossible
	if (points.size() < 3)
		return std::nullopt;

	// make a temp copy to allow use swap
	std::vector<ImVec2> v{ points.begin(), points.end() };

	// now at index 0 we have most left point
	std::swap(v.at(0), *std::min_element(v.begin(), v.end(),
		// is left? a lexicographically before b
		[](const ImVec2& a, const ImVec2& b) constexpr
		{
			return (a.x < b.x || (a.x == b.x && a.y < b.y));
		}
	));
	
	std::vector<ImVec2> hull;

	auto p0 = v.at(0);
	auto h0 = hull.at(0);
	do {
		hull.push_back(p0);
		std::swap(p0, *std::min_element(v.begin() + 1, v.end(),
			[p0](const ImVec2& a, const ImVec2& b) constexpr
			{
				return orient(p0, a, b) < 0.0f;
			}
		));
	} while (p0.x != h0.x && p0.y != h0.y); // when it is point[0]

	return hull;
}