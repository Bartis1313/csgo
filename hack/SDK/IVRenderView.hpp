#pragma once

#include "helpers/vfunc.hpp"

class IMaterial;

class IVRenderView
{
public:
	VFUNC(void, setBlend, 4, (float blend), (this, blend));
	VFUNC(float, getBlend, 5, (), (this));
	VFUNC(void, modulateColor, 6, (float const* blend), (this, blend));
	VFUNC(void, getcolor, 7, (float* blend), (this, blend));
};