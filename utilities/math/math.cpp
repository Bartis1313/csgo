#include "math.hpp"

Vector math::calcAngleRelative(const Vector& source, const Vector& destination, const Vector& viewAngles)
{
	const auto delta = destination - source;

	Vector angles(
		RAD2DEG(std::atan2(-delta.z, std::hypot(delta.x, delta.y))) - viewAngles.x,
		RAD2DEG(std::atan2(delta.y, delta.x)) - viewAngles.y,
		0.0f
	);
	return angles.normalize();
}

Vector math::calcAngle(const Vector& source, const Vector& destination)
{
	const auto delta = destination - source;

	Vector angles(
		RAD2DEG(std::atan2(-delta.z, std::hypot(delta.x, delta.y))),
		RAD2DEG(std::atan2(delta.y, delta.x)),
		0.0f
	);

	return angles.normalize();
}

float math::calcFov(const Vector& source, const Vector& destination, const Vector& viewAngles)
{
	const auto angle = calcAngleRelative(source, destination, viewAngles);
	const auto fov = std::hypot(angle.x, angle.y);
	return fov;
}

float math::calcFovReal(const Vector& source, const Vector& destination, const Vector& viewAngles)
{
	float dist = source.distTo(destination);
	float realDist = dist / 10.0f;

	auto start = angleVec(viewAngles) * realDist;
	auto end = angleVec(calcAngle(source, destination)) * realDist;

	return start.distTo(end);
}

Vector math::transformVector(const Vector& in, const Matrix3x4& matrix)
{
	return Vector(in.dot(matrix[0]) + matrix[0][3], in.dot(matrix[1]) + matrix[1][3],
		in.dot(matrix[2]) + matrix[2][3]);
}

Vector math::angleVec(const Vector& angle)
{
	auto sy = std::sin(DEG2RAD(angle.y));
	auto cy = std::cos(DEG2RAD(angle.y));

	auto sp = std::sin(DEG2RAD(angle.x));
	auto cp = std::cos(DEG2RAD(angle.x));

	return Vector(cp * cy, cp * sy, -sp);
}

std::tuple<Vector, Vector, Vector> math::angleVectors(const Vector& angle)
{
	auto sy = std::sin(DEG2RAD(angle.y));
	auto cy = std::cos(DEG2RAD(angle.y));

	auto sp = std::sin(DEG2RAD(angle.x));
	auto cp = std::cos(DEG2RAD(angle.x));

	auto sr = std::sin(DEG2RAD(angle.z));
	auto cr = std::cos(DEG2RAD(angle.z));

	Vector forward;
	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;

	Vector right;
	right.x = (-1.0f * sr * sp * cy + -1.0f * cr * -sy);
	right.y = (-1.0f * sr * sp * sy + -1.0f * cr * cy);
	right.z = -1.0f * sr * cp;

	Vector up;
	up.x = (cr * sp * cy + -sr * -sy);
	up.y = (cr * sp * sy + -sr * cy);
	up.z = cr * cp;

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

Vector math::vectorToAngle(const Vector& vec)
{
	Vector angle = {};
	if (vec.x == 0.0f && vec.y == 0.0f)
	{
		angle.x = (vec.z > 0.0f) ? 270.0f : 90.0f;
		angle.y = 0.0f;
	}
	else
	{
		angle.x = RAD2DEG(std::atan2(-vec.z, std::hypot(vec.x, vec.y)));
		angle.y = RAD2DEG(std::atan2(vec.y, vec.x));

		if (angle.y > 90.0f)
			angle.y -= 180.0f;
		else if (angle.y < 90.0f)
			angle.y += 180.0f;
		else if (angle.y == 90.0f)
			angle.y = 0.0f;
	}
	angle.z = 0.0f;
	return angle;
}

std::pair<Vector, Vector> math::transformAABB(const Matrix3x4& transform, const Vector& mins, const Vector& maxs)
{
	Vector localCenter = mins + maxs;
	localCenter *= 0.5f;

	Vector localExtents = maxs - localCenter;

	Vector worldCenter = transformVector(localCenter, transform);

	Vector worldExtents;
	worldExtents.x = localExtents.dot(transform[0]);
	worldExtents.y = localExtents.dot(transform[1]);
	worldExtents.z = localExtents.dot(transform[2]);

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

std::vector<ImVec2> math::grahamScan(std::span<const ImVec2> points)
{
	// case when it's impossible
	if (points.size() < 3)
		return {};

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
		[v = v](const ImVec2& a, const ImVec2& b) constexpr
		{
			return orient(v.at(0), a, b) < 0.0f;
		}
	);

	std::vector<ImVec2> hull;

	// always push 3 to start hull
	auto it = v.begin();
	hull.push_back(*it++);
	hull.push_back(*it++);
	hull.push_back(*it++);

	while (it != v.end()) 
	{
		// we make angle with convex?
		while (orient(*(hull.rbegin() + 1), *(hull.rbegin()), *it) >= 0.0f)
			hull.pop_back();

		hull.push_back(*it++);
	}

	return hull;
}

std::vector<ImVec2> math::giftWrap(std::span<const ImVec2> points)
{
	// case when it's impossible
	if (points.size() < 3)
		return {};

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

	// first to last hull
	do {
		hull.push_back(v.at(0));
		std::swap(v[0], *std::min_element(v.begin() + 1, v.end(),
			[v = v](const ImVec2& a, const ImVec2& b) constexpr
			{
				return orient(v.at(0), a, b) < 0.0f;
			}
		));
	} while (v.at(0).x != hull.at(0).x && v.at(0).y != hull.at(0).y); // when it is point[0]

	return hull;
}