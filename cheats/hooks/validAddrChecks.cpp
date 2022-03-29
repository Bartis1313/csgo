#include "hooks.hpp"
#include "../../config/vars.hpp"

// https://imgur.com/KdBfQD3 - useless to add this random addr, and easier for everyone, I force to return valid addr

char __fastcall hooks::clientValidAddr::hooked(FAST_ARGS, const char* lpModuleName)
{
	return 1;
}

char __fastcall hooks::engineValidAddr::hooked(FAST_ARGS, const char* lpModuleName)
{
	return 1;
}

char __fastcall hooks::studioRenderValidAddr::hooked(FAST_ARGS, const char* lpModuleName)
{
	return 1;
}

char __fastcall hooks::materialSystemValidAddr::hooked(FAST_ARGS, const char* lpModuleName)
{
	return 1;
}

bool __fastcall hooks::isUsingStaticPropDebugModes::hooked(FAST_ARGS)
{
	return config.get<bool>(vars.bModulateColor); // might check ret addr, but pretty useless imho
}