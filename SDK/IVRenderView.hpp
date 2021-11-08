#pragma once
#include "../utilities/vfunc.hpp"

class IViewRender
{
public:
	VFUNC(void, setBlend, 4, (float blend), (this, blend));
	VFUNC(float, getblend, 5, (), (this));
	VFUNC(void, modulatecolor, 6, (float const* blend), (this, blend));
	VFUNC(void, getcolor, 7, (float* blend), (this, blend));
};