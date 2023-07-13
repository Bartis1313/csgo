#include "decalAddToSurface.hpp"

hooks::R_AddDecalToSurface::value hooks::R_AddDecalToSurface::hook(void* a1, void* a2, void* a3, void* a4)
{
	//printf("called\n");

	return original(a1, a2, a3, a4);
}