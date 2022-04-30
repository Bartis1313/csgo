#pragma once

#include "math/Vector.hpp"

class CEffectData
{
public:
	Vector m_origin = {};
	Vector m_start = {};
	Vector m_normal = {};
	Vector m_angles = {};
	int m_flags = 0;
	float m_scale = 1.0f;
	float m_magnitude = 0.0f;
	float m_radius = 0.0f;
	int m_attachmentIndex = 0;
	short m_surfaceProp = 0;
	// Some TF2 specific things
	int m_material = 0;
	int m_damageType = 0;
	int m_hitBox = 0;
	int m_otherEntIndex = 0;
	unsigned char m_color = 0;
	bool m_positionsAreRelativeToEntity = false;
	int getEffectNameIndex() { return m_iEffectName; }
private:
	int m_iEffectName;	// Entry in the EffectDispatch network string table. The is automatically handled by DispatchEffect().
};
