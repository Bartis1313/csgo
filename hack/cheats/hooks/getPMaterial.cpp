#include "getPMaterial.hpp"

// called on init only, as cached materials
hooks::GetPMaterial::value hooks::GetPMaterial::hook(HACK_FAST_ARGS, const char* name)
{
#ifdef _DEBUG
	//printf("name %s\n", name);
#endif

	return original(thisptr, name);
}