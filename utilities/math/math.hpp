#pragma once
#include "../../SDK/math/Vector.hpp"
#include "../../SDK/math/VMatrix.hpp"

// read: https://www.unknowncheats.me/forum/counterstrike-global-offensive/137492-math-behind-hack-1-coding-better-aimbot-stop-using-calcangle.html

namespace math
{
	Vector calcAngleRelative(const Vector& src, const Vector& dest, const Vector& viewAngle);
	float calcFov(const Vector& source, const Vector& destination, const Vector& viewAngles);	
	// based on two vectors resulting one
	void makeVec(const Vector& in, Vector& out);
	Vector transformVector(const Vector& in, matrix3x4_t matrix);
	void transformVector(Vector& in, matrix3x4_t& mat, Vector& out);
	Vector angleVec(const Vector& angle);
}