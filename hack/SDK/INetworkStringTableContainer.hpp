#pragma once

#include "helpers/vfunc.hpp"
#include "IVModelInfo.hpp"
#include "interfaces/interfaces.hpp"

#define INVALID_STRING_TABLE -1
const unsigned short INVALID_STRING_INDEX = (unsigned short)-1;

class INetworkStringTable
{
public:
	VFUNC(int, addString, 8, (bool isServer, const char* value, int lenght = -1, const void* userdata = nullptr), (this, isServer, value, lenght, userdata));
};

class INetworkStringTableContainer
{
public:
	VFUNC(INetworkStringTable*, findTable, 3, (const char* name), (this, name));
};

// https://www.unknowncheats.me/forum/counterstrike-global-offensive/214919-precache-models.html
inline bool precacheModel(const char* name)
{
	INetworkStringTable* table = memory::interfaces::clientStringTableContainer->findTable("modelprecache");

	if (table)
	{
		memory::interfaces::modelInfo->findOrLoadModel(name);

		int idx = table->addString(false, name);

		if (idx != INVALID_STRING_INDEX)
			return true;
	}
	return false;
}