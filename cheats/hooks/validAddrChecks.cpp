#include "hooks.hpp"

// https://imgur.com/KdBfQD3 - useless to add this random addr, and easier for everyone, I force to return valid addr

char __fastcall hooks::clientValidAddr::hooked(void* thisptr, void* edx, const char* lpModuleName)
{
	return 1;
}

char __fastcall hooks::engineValidAddr::hooked(void* thisptr, void* edx, const char* lpModuleName)
{
	return 1;
}

char __fastcall hooks::studioRenderValidAddr::hooked(void* thisptr, void* edx, const char* lpModuleName)
{
	return 1;
}

char __fastcall hooks::materialSystemValidAddr::hooked(void* thisptr, void* edx, const char* lpModuleName)
{
	return 1;
}