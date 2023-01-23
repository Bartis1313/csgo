#pragma once

#include "helpers/pad.hpp"

using getSymbolProc_t = bool(__cdecl*)(const char*);

class KeyValues
{
public:
	KeyValues(const char* key, const char* maybeSomePreValue = nullptr, int caseSensitive = 0);
	~KeyValues();
	static KeyValues* fromString(const char* name, const char* value = nullptr);
	void fromBuffer(const char* name, const char* buffer, void* filesystem = nullptr, const char* pathId = nullptr, void* symbolProc = nullptr, int unk = 0);
	void* operator new(size_t size);

	PAD(0x2C); // keep this updated to prevent calling size allocation yourself manually
};
static_assert(sizeof(KeyValues) == 0x2C);