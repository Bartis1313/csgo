#include "findMaterial.hpp"

#include <cheats/features/visuals/world/weather/ground.hpp>

hooks::FindMaterial::value hooks::FindMaterial::hook(FAST_ARGS, char const* name, const char* group, bool complain, const char* prefix)
{
	const std::string_view sName = std::string_view{ name };

	auto mat = original(thisptr, name, group, complain, prefix);
	auto custom = weather::ground::applyFindMat(mat);

	return custom ? custom : mat;
}