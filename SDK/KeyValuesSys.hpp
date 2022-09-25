#pragma once

#include <utilities/vfunc.hpp>

class KeyValuesSys
{
public:
	VFUNC(void*, allocKeysMemory, 2, (size_t size), (this, size));
	VFUNC(void, freeKeysMemory, 3, (void* memory), (this, memory));
	VFUNC(int, getSymbolForString, 4, (const char* name, bool create = true), (this, name, create));
};