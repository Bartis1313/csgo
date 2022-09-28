#pragma once

#include "math/Vector.hpp"

class CEffectData
{
public:
	Vec3 m_origin;
	Vec3 m_start;
	Vec3 m_normal;
	Vec3 m_angles;
	int m_flags;
	int m_entityIdx;
	float m_scale;
	float m_magnitude;
	float m_radius;
	int m_attachmentIndex;
	short m_surfaceProp;
	// Some TF2 specific things
	int m_material;
	int m_damageType;
	int m_hitBox;
	int m_otherEntIndex;
	unsigned char m_color;
	int getEffectNameIndex() { return m_iEffectName; }
private:
	int m_iEffectName;	// Entry in the EffectDispatch network string table. The is automatically handled by DispatchEffect().
};