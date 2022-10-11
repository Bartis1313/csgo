#include "modulateColors.hpp"

#include <SDK/IMaterialSystem.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>
#include <game/globals.hpp>

#include <unordered_map>

void ModulateColor::run(void* thisptr, float* r, float* g, float* b)
{
	if (!vars::visuals->world->modulate->enabled)
		return;

	auto editColor = [=](CfgColor* color)
	{
		*r = color->getColor().r();
		*g = color->getColor().g();
		*b = color->getColor().b();
		color->refresh();
	};

	auto material = reinterpret_cast<IMaterial*>(thisptr);

	if (!material)
		return;

	if (material->isError())
		return;

	std::string_view name = material->getTextureGroupName();

	static std::unordered_map<std::string_view, CfgColor*> mapped =
	{
		{ XOR("World textures"), &vars::visuals->world->modulate->texture },
		{ XOR("StaticProp textures"), &vars::visuals->world->modulate->prop },
		{ XOR("SkyBox textures"), &vars::visuals->world->modulate->sky },
	};

	bool goodMat = false;

	if (auto itr = mapped.find(name); itr != mapped.end())
	{
		editColor(itr->second);
		goodMat = true;
	}

	constexpr int shaderAlpha = 5;

	if (auto shader = material->getShaderParams()[shaderAlpha]; goodMat)
	{
		!globals::isShutdown
			? shader->setValue(vars::visuals->world->modulate->shader / 100.0f)
			: shader->setValue(1.0f); // default val reset
	}
}