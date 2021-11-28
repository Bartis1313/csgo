#include "math.hpp"

Vector math::calcAngleRelative(const Vector& source, const Vector& destination, const Vector& viewAngles)
{
	const auto delta = destination - source;

	Vector angles(
		RAD2DEG(std::atan2(-delta.z , std::hypot(delta.x, delta.y))) - viewAngles.x,
		RAD2DEG(std::atan2(delta.y, delta.x)) - viewAngles.y,
		0.0f
	);

	angles.normalize();
	return angles;
}

float math::calcFov(const Vector& source, const Vector& destination, const Vector& viewAngle)
{
	const auto angle = calcAngleRelative(std::cref(source), std::cref(destination), std::cref(viewAngle));
	const auto fov = std::hypot(angle.x, angle.y);
	return fov;
}

void math::makeVec(const Vector& in, Vector& out)
{
	const auto pitch = DEG2RAD(in.x);
	const auto yaw = DEG2RAD(in.y);
	const auto cospitch = std::cos(pitch);

	out.x = -cospitch * -std::cos(yaw);
	out.y = std::sin(yaw) * cospitch;
	out.z = -std::sin(pitch);
}

Vector math::transformVector(const Vector& in, matrix3x4_t matrix)
{
	return Vector(in.Dot(matrix[0]) + matrix[0][3], in.Dot(matrix[1]) + matrix[1][3],
		in.Dot(matrix[2]) + matrix[2][3]);
}

void math::transformVector(Vector& in, matrix3x4_t& mat, Vector& out)
{
	out.x = in.Dot(mat.m_flMatVal[0]) + mat.m_flMatVal[0][3];
	out.y = in.Dot(mat.m_flMatVal[1]) + mat.m_flMatVal[1][3];
	out.z = in.Dot(mat.m_flMatVal[2]) + mat.m_flMatVal[2][3];
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
		angle.x = RAD2DEG(std::atan2(-vec.z, vec.Length2D()));
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