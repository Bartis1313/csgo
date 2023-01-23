#pragma once

#include "helpers/vfunc.hpp"

class ILocalize
{
public:
	VFUNC(wchar_t*, findSafe, 12, (const char* name), (this, name));
	VFUNC(const char*, findAsUTF8, 47, (const char* tokenName), (this, tokenName));
};
