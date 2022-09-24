#pragma once

using getSymbolProc_t = bool(__cdecl*)(const char*);

class KeyValues
{
public:
	static KeyValues* fromString(const char* name, const char* value = nullptr);
};