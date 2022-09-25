#pragma once

#include <utilities/vfunc.hpp>

class IPanel
{
public:
	VFUNC(const char*, getName, 36, (unsigned int panel), (this, panel));
	VFUNC(void, setMouseInputEnabled, 32, (unsigned int panel, bool state), (this, panel, state));
};