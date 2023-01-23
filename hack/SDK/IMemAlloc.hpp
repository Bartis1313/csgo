#pragma once

#include "helpers/vfunc.hpp"

class IMemAlloc
{
public:
	VFUNC(void*, _alloc, 1, (size_t size), (this, size));
};