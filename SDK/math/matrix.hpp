#pragma once
#include "Vector.hpp"
#include <compare>

struct matrix3x4_t
{
	matrix3x4_t() = default;
	constexpr matrix3x4_t(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23)
	{
		m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
	}

	constexpr auto operator<=>(const matrix3x4_t&) const = default;

	_NODISCARD constexpr Vector origin() const
	{
		return Vector{ m_flMatVal[0][3], m_flMatVal[1][3], m_flMatVal[2][3] };
	}

	constexpr const float* operator[](int i) const
	{
		return m_flMatVal[i];
	}

	constexpr float* operator[](int i)
	{
		return m_flMatVal[i];
	}

	float m_flMatVal[3][4];
};

struct VMatrix
{
	VMatrix() = default;
	constexpr VMatrix(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
	{
		m_flMatVal[0][0] = m00;	m_flMatVal[0][1] = m01; m_flMatVal[0][2] = m02; m_flMatVal[0][3] = m03;
		m_flMatVal[1][0] = m10;	m_flMatVal[1][1] = m11; m_flMatVal[1][2] = m12; m_flMatVal[1][3] = m13;
		m_flMatVal[2][0] = m20;	m_flMatVal[2][1] = m21; m_flMatVal[2][2] = m22; m_flMatVal[2][3] = m23;
		m_flMatVal[3][0] = m30;	m_flMatVal[3][1] = m31; m_flMatVal[3][2] = m32; m_flMatVal[3][3] = m33;
	}

	constexpr float* operator[](int i)
	{
		return m_flMatVal[i];
	}

	constexpr const float* operator[](int i) const
	{
		return m_flMatVal[i];
	}

	float m_flMatVal[4][4];
};