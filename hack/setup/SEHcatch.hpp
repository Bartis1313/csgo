#pragma once

#include <Windows.h>

namespace SEHcatch
{
	LONG WINAPI memErrorCatch(EXCEPTION_POINTERS* pExceptionInfo);
};