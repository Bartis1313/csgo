#pragma once
#include "../utilities/vfunc.hpp"

class IPanel
{
public:
	VFUNC(const char*, getName, 36, (unsigned int panel), (this, panel));
};