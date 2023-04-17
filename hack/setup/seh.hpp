#pragma once

#include <Windows.h>

namespace SEH
{
	LONG WINAPI CATCH(EXCEPTION_POINTERS* pExceptionInfo);
}