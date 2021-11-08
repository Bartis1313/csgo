#pragma once
#include "../utilities/vfunc.hpp"

class ILocalize
{
public:
	VFUNC(wchar_t*, findSafe, 12, (const char* name), (this, name));
};
