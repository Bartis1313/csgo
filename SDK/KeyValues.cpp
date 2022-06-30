#include "KeyValues.hpp"

#include "structs/IDXandPaterrns.hpp"
#include "../utilities/utilities.hpp"
#include "KeyValuesSys.hpp"
#include "interfaces/interfaces.hpp"

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
