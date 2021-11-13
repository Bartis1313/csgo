#pragma once
#include "math/Vector.hpp"
#include "Color.hpp"

enum
{
	DLIGHT_NO_WORLD_ILLUMINATION = 0x1,
	DLIGHT_NO_MODEL_ILLUMINATION = 0x2,

	// NOTE: These two features are used to dynamically tweak the alpha on displacements
	// which is a special effect for selecting which texture to use. If 
	// we ever change how alpha is stored for displacements, we'll have to kill this feature
	DLIGHT_ADD_DISPLACEMENT_ALPHA = 0x4,
	DLIGHT_SUBTRACT_DISPLACEMENT_ALPHA = 0x8,
	DLIGHT_DISPLACEMENT_MASK = (DLIGHT_ADD_DISPLACEMENT_ALPHA | DLIGHT_SUBTRACT_DISPLACEMENT_ALPHA),
};

/*struct ColorRGBExp32
{
	unsigned char r, g, b;
	signed char exponent;
};
*/

struct DLight_t
{
	int	m_flags;
	Vector m_origin;
	float m_radius;
	Color m_color;
	float m_die;
	float m_decay;
	float m_minlight;
	int	m_key;
	int	m_style;
	Vector m_Direction;
	float m_InnerAngle;
	float m_OuterAngle;
};

class IVEfx
{
public:
	VFUNC(DLight_t*, clAllocDlight, 4, (int key), (this, key));
	VFUNC(DLight_t*, clAllocElight, 5, (int key), (this, key));
	VFUNC(DLight_t*, getElightbyKey, 8, (int key), (this, key));
};