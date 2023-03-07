#include "material.hpp"

#include <gamememory/memory.hpp>

void IMaterial::destroy()
{
	memory::destroyMaterial()(this);
}