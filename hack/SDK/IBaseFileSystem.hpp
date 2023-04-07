#pragma once

#include "helpers/vfunc.hpp"

class IBaseFileSystem
{
public:
	VFUNC(int, read, 0, (void* out, int size, void* file), (this, out, size, file));
	VFUNC(void*, open, 2, (const char* file, const char* options, const char* path), (this, file, options, path));
	VFUNC(void, close, 3, (void* file), (this, file));
	VFUNC(int, size, 7, (void* file), (this, file));
};