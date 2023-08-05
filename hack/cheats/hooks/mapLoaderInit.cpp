#include "mapLoaderInit.hpp"

hooks::MapLoaderInit::value hooks::MapLoaderInit::hook(FAST_ARGS, model_t* model, const char* pathName)
{
	printf("path name %s\n", pathName);

	original(thisptr, model, pathName);
}