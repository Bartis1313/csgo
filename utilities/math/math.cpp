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

void math::angleVectors(const Vector& angle, Vector& forward, Vector& right, Vector& up)
{
	auto sy = std::sin(DEG2RAD(angle.y));
	auto cy = std::cos(DEG2RAD(angle.y));

	auto sp = std::sin(DEG2RAD(angle.x));
	auto cp = std::cos(DEG2RAD(angle.x));

	auto sr = std::sin(DEG2RAD(angle.z));
	auto cr = std::cos(DEG2RAD(angle.z));

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;

	right.x = (-1.0f * sr * sp * cy + -1.0f * cr * -sy);
	right.y = (-1.0f * sr * sp * sy + -1.0f * cr * cy);
	right.z = -1.0f * sr * cp;

	up.x = (cr * sp * cy + -sr * -sy);
	up.y = (cr * sp * sy + -sr * cy);
	up.z = cr * cp;
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

float math::customSin(float time, float multiply)
{
	constexpr float RAD_PI_2 = 2.0f * PI;
	float a = time / RAD_PI_2;
	float angle = time - std::floor(a) * RAD_PI_2;
	if (angle >= RAD_PI_2)
		angle = 0.0f;

	float res = (0.5f * std::sin(angle) + 0.5f) * multiply;
	return res;
}