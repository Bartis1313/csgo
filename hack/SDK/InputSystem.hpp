#pragma once

#include "helpers/vfunc.hpp"

class InputSystem
{
public:
	VFUNC(void, enableInput, 11, (bool enable), (this, enable));
};