#include "hooks.hpp"
#include "../../SDK/structs/IDXandPaterrns.hpp"
#include "../../utilities/utilities.hpp"

#ifdef _DEBUG
	#define GET_RANDOM_ADDR(_module) reinterpret_cast<uintptr_t>(GetModuleHandleA(_module) + 0xFFF + utilities::getRandomAddr(0, 0x4444))
#else
	#define GET_RANDOM_ADDR(_module) reinterpret_cast<uintptr_t>(LF(GetModuleHandleA).cached()(_module) + 0xFFF + utilities::getRandomInt(0, 0x4444))
#endif

char __fastcall hooks::clientValidAddr::hooked(uintptr_t thisptr, int edx, uintptr_t retAddr)
{
	static uintptr_t randomAddr = GET_RANDOM_ADDR(CLIENT_DLL);
	return original(thisptr, edx, randomAddr);
}

char __fastcall hooks::engineValidAddr::hooked(uintptr_t thisptr, int edx, uintptr_t retAddr)
{
	static uintptr_t randomAddr = GET_RANDOM_ADDR(ENGINE_DLL);
	return original(thisptr, edx, randomAddr);
}

char __fastcall hooks::studioRenderValidAddr::hooked(uintptr_t thisptr, int edx, uintptr_t retAddr)
{
	static uintptr_t randomAddr = GET_RANDOM_ADDR(STUDIORENDER_DLL);
	return original(thisptr, edx, randomAddr);
}

char __fastcall hooks::materialSystemValidAddr::hooked(uintptr_t thisptr, int edx, uintptr_t retAddr)
{
	static uintptr_t randomAddr = GET_RANDOM_ADDR(MATERIAL_DLL);
	return original(thisptr, edx, randomAddr);
}