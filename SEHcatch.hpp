#pragma once

#include <Windows.h>

class SEHcatch
{
public:
	SEHcatch() = delete;
	static LONG WINAPI memErrorCatch(EXCEPTION_POINTERS* pExceptionInfo);
private:
	static void printStack();
	static constexpr uint32_t MAX_FRAMES = 1024;
	static constexpr uint32_t MAX_LENGTH = 1024;
};