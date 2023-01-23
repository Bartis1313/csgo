#pragma once

#include "EHandle.hpp"
#include "helpers/vfunc.hpp"

class IClientEntityList
{
public:
	VFUNC(void*, getClientEntity, 3, (int index), (this, index));
	VFUNC(void*, getClientFromHandle, 4, (EHandle_t handle), (this, handle.handle));
	VFUNC(int, numberOfEntites, 5, (bool includeNonNetw), (this, includeNonNetw));
	VFUNC(int, getHighestIndex, 6, (), (this));
	VFUNC(void, setMaxEntites, 7, (int num), (this, num));
	VFUNC(int, getMaxEntities, 8, (), (this));
};