#pragma once
#include "material.hpp"
#include "../utilities/vfunc.hpp"

class IMaterial;
class KeyValues;

class IMaterialSystem
{
public:
	VFUNC(IMaterial*, createMaterial, 83, (const char* materialName, KeyValues* keyValues), (this, materialName, keyValues))
	VFUNC(IMaterial*, findMaterial, 84, (char const* name, const char* group, bool complain = true, const char* prefix = 0), (this, name, group, complain, prefix));
	VFUNC(short, firstMaterial, 86, (), (this));
	VFUNC(short, nextMaterial, 87, (int handle), (this, handle));
	VFUNC(short, invalidMaterialFromHandle, 88, (), (this));
	VFUNC(IMaterial*, getMaterial, 89, (short handle), (this, handle));
	VFUNC(int, getMaterialCount, 90, (), (this));
};
