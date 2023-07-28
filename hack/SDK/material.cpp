#include "material.hpp"

#include <gamememory/memory.hpp>

void IMaterial::destroy()
{
	memory::destroyMaterial()(this);
}

bool IMaterial::precacheVars(void* vmtValues, void* patchValues, void* includes)
{
	return memory::precacheVars()(this, vmtValues, patchValues, includes);
}