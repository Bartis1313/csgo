#include "KeyValues.hpp"

#include "KeyValuesSys.hpp"
#include "interfaces/interfaces.hpp"

#include <gamememory/memory.hpp>

KeyValues* KeyValues::fromString(const char* name, const char* value)
{
	const static auto fromstring = g_Memory.m_keyValuesFromString();
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
