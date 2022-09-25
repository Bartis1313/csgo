#pragma once

#include "Vector.hpp"

struct Matrix3x4
{
	Matrix3x4() = default;
	constexpr Matrix3x4(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23)
	{
		m_matValues[0][0] = m00; m_matValues[0][1] = m01; m_matValues[0][2] = m02; m_matValues[0][3] = m03;
		m_matValues[1][0] = m10; m_matValues[1][1] = m11; m_matValues[1][2] = m12; m_matValues[1][3] = m13;
		m_matValues[2][0] = m20; m_matValues[2][1] = m21; m_matValues[2][2] = m22; m_matValues[2][3] = m23;
	}

	[[nodiscard]] constexpr Vector origin() const
	{
		return Vector{ m_matValues[0][3], m_matValues[1][3], m_matValues[2][3] };
	}

	constexpr const float* operator[](int i) const
	{
		return m_matValues[i];
	}

	constexpr float* operator[](int i)
	{
		return m_matValues[i];
	}

	float m_matValues[3][4];
};

struct Matrix4x4
{
	Matrix4x4() = default;
	constexpr Matrix4x4(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
	{
		m_matValues[0][0] = m00; m_matValues[0][1] = m01; m_matValues[0][2] = m02; m_matValues[0][3] = m03;
		m_matValues[1][0] = m10; m_matValues[1][1] = m11; m_matValues[1][2] = m12; m_matValues[1][3] = m13;
		m_matValues[2][0] = m20; m_matValues[2][1] = m21; m_matValues[2][2] = m22; m_matValues[2][3] = m23;
		m_matValues[3][0] = m30; m_matValues[3][1] = m31; m_matValues[3][2] = m32; m_matValues[3][3] = m33;
	}

	constexpr float* operator[](int i)
	{
		return m_matValues[i];
	}

	constexpr const float* operator[](int i) const
	{
		return m_matValues[i];
	}

	float m_matValues[4][4];
};