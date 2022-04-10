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
	const auto delta = source - destination;

	Vector angles(
		RAD2DEG(std::atanf(delta.z / std::hypotf(delta.x, delta.y))),
		RAD2DEG(std::atanf(delta.y / delta.x)),
		0.0f
	);

	angles.z = 0.0f;
	if (delta.x >= 0.0)
		angles.y += 180.0f;

	return angles;
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
		angle.x = RAD2DEG(std::atan2(-vec.z, vec.length2D()));
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