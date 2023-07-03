#pragma once

#include "math/Vector.hpp"

enum LightType_t
{
	MATERIAL_LIGHT_DISABLE = 0,
	MATERIAL_LIGHT_POINT,
	MATERIAL_LIGHT_DIRECTIONAL,
	MATERIAL_LIGHT_SPOT,
};

enum LightType_OptimizationFlags_t
{
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION0 = 1,
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION1 = 2,
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION2 = 4,
	LIGHTTYPE_OPTIMIZATIONFLAGS_DERIVED_VALUES_CALCED = 8,
};

struct LightDesc_t
{
	LightType_t m_Type;										//< MATERIAL_LIGHT_xxx
	Vec3 m_Color;											//< color+intensity 
	Vec3 m_Position;										//< light source center position
	Vec3 m_Direction;										//< for SPOT, direction it is pointing
	float m_Range;											//< distance range for light.0=infinite
	float m_Falloff;										//< angular falloff exponent for spot lights
	float m_Attenuation0;									//< constant distance falloff term
	float m_Attenuation1;									//< linear term of falloff
	float m_Attenuation2;									//< quadatic term of falloff

	// NOTE: theta and phi are *half angles*
	float m_Theta;											//< inner cone angle. no angular falloff 
	//< within this cone
	float m_Phi;											//< outer cone angle

	// the values below are derived from the above settings for optimizations
	// These aren't used by DX8. . used for software lighting.

	// NOTE: These dots are cos( m_Theta ), cos( m_Phi )
	float m_ThetaDot;
	float m_PhiDot;
	float m_OneOverThetaDotMinusPhiDot;
	unsigned int m_Flags;
protected:
	float m_RangeSquared;
public:

	//void RecalculateDerivedValues(void);			 // calculate m_xxDot, m_Type for changed parms
	//void RecalculateOneOverThetaDotMinusPhiDot();

	//LightDesc_t(void)
	//{
	//}

	//// constructors for various useful subtypes

	//// a point light with infinite range
	//LightDesc_t(const Vector& pos, const Vector& color)
	//{
	//	InitPoint(pos, color);
	//}

	//LightDesc_t& operator=(const LightDesc_t& src)
	//{
	//	memcpy(this, &src, sizeof(LightDesc_t));
	//	return *this;
	//}

	///// a simple light. cone boundaries in radians. you pass a look_at point and the
	///// direciton is derived from that.
	//LightDesc_t(const Vector& pos, const Vector& color, const Vector& point_at,
	//	float inner_cone_boundary, float outer_cone_boundary)
	//{
	//	InitSpot(pos, color, point_at, inner_cone_boundary, outer_cone_boundary);
	//}

	//void InitPoint(const Vector& pos, const Vector& color);
	//void InitDirectional(const Vector& dir, const Vector& color);
	//void InitSpot(const Vector& pos, const Vector& color, const Vector& point_at,
	//	float inner_cone_boundary, float outer_cone_boundary);

	///// Given 4 points and 4 normals, ADD lighting from this light into "color".
	//void ComputeLightAtPoints(const FourVectors& pos, const FourVectors& normal,
	//	FourVectors& color, bool DoHalfLambert = false) const;
	//void ComputeNonincidenceLightAtPoints(const FourVectors& pos, FourVectors& color) const;
	//void ComputeLightAtPointsForDirectional(const FourVectors& pos,
	//	const FourVectors& normal,
	//	FourVectors& color, bool DoHalfLambert = false) const;

	//// warning - modifies color!!! set color first!!
	//void SetupOldStyleAttenuation(float fQuadatricAttn, float fLinearAttn, float fConstantAttn);

	//void SetupNewStyleAttenuation(float fFiftyPercentDistance, float fZeroPercentDistance);


	///// given a direction relative to the light source position, is this ray within the
	//	/// light cone (for spotlights..non spots consider all rays to be within their cone)
	//bool IsDirectionWithinLightCone(const Vector& rdir) const
	//{
	//	return ((m_Type != MATERIAL_LIGHT_SPOT) || (rdir.Dot(m_Direction) >= m_PhiDot));
	//}

	//float OneOverThetaDotMinusPhiDot() const
	//{
	//	return m_OneOverThetaDotMinusPhiDot;
	//}

	//float DistanceAtWhichBrightnessIsLessThan(float flAmount) const;
};
