#include "findMaterial.hpp"

#include <cheats/features/visuals/chams/factory/factory.hpp>

hooks::FindMaterial::value hooks::FindMaterial::hook(FAST_ARGS, char const* name, const char* group, bool complain, const char* prefix)
{
	const std::string_view sName = std::string_view{ name };

	return original(thisptr, name, group, complain, prefix);
}