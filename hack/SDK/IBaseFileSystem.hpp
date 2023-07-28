#pragma once

#include "helpers/vfunc.hpp"
#include "CutlBuffer.hpp"

class IBaseFileSystem
{
public:
	VFUNC(int, read, 0, (void* out, int size, void* file), (this, out, size, file));
	VFUNC(int, write, 0, (void* const in, int size, void* file), (this, in, size, file));
	VFUNC(void*, open, 2, (const char* file, const char* options, const char* path), (this, file, options, path));
	VFUNC(void, close, 3, (void* file), (this, file));
	VFUNC(int, size, 7, (void* file), (this, file));
	VFUNC(bool, writeFile, 15, (const char* name, const char* path, CUtlBuffer& buffer), (this, name, path, buffer));
};