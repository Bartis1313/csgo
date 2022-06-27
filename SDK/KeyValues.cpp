#include "KeyValues.hpp"

#include "structs/IDXandPaterrns.hpp"
#include "../utilities/utilities.hpp"
#include "KeyValuesSys.hpp"
#include "interfaces/interfaces.hpp"

KeyValues::KeyValues(const char* name)
{
	using fn = void(__thiscall*)(void*, const char*, void*, int);
	const static auto construct = reinterpret_cast<fn>(utilities::patternScan(CLIENT_DLL, KEY_VALUES_CONSTRUCT));
	construct(this, name, nullptr, -1);
}

KeyValues* KeyValues::fromString(const char* name, const char* value)
{
	const static auto fromstring = utilities::patternScan(CLIENT_DLL, KEY_VALUES_FROM_STR);
	KeyValues* res;

	__asm
	{
		push 0
		mov edx, value
		mov ecx, name
		call fromstring
		add esp, 4
		mov res, eax
	}

	return res;
}

void KeyValues::loadFromBuffer(char const* resourceName, const char* buffer, void* fileSys,
	const char* pathID, getSymbolProc_t evaluateSymbolProc)
{
	using fn = void(__thiscall*)(void*, const char*, const char*, void*, const char*, void*);
	const static auto loadbuf = reinterpret_cast<fn>(utilities::patternScan(CLIENT_DLL, KEY_VALUES_LOAD_BUF));
	loadbuf(this, resourceName, buffer, fileSys, pathID, evaluateSymbolProc);
}

void* KeyValues::operator new(size_t size)
{
	return interfaces::keyValuesSys->allocKeysMemory(size);
}

void KeyValues::operator delete(void* mem)
{
	interfaces::keyValuesSys->freeKeysMemory(mem);
}