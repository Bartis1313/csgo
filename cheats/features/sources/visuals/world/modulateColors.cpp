#include "modulateColors.hpp"

#include <SDK/IMaterialSystem.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>

#include <unordered_map>

void ModulateColor::run(void* thisptr, float* r, float* g, float* b)
{
	if (!config.get<bool>(vars.bModulateColor))
		return;

	auto editColor = [=](const CfgColor& color)
	{
		*r = color.getColor().r();
		*g = color.getColor().g();
		*b = color.getColor().b();
	};

	auto material = reinterpret_cast<IMaterial*>(thisptr);

	if (!material)
		return;

	if (material->isError())
		return;

	std::string_view name = material->getTextureGroupName();

	static std::unordered_map<std::string_view, CfgColor*> mapped =
	{
		{ XOR("World textures"), &config.getRef<CfgColor>(vars.cWorldTexture) },
		{ XOR("StaticProp textures"), &config.getRef<CfgColor>(vars.cWorldProp) },
		{ XOR("SkyBox textures"), &config.getRef<CfgColor>(vars.cSkyBox) },
	};

	if (auto itr = mapped.find(name); itr != mapped.end())
		editColor(*itr->second);

	//constexpr int shaderAlpha = 5;

	//// this might be bugged, remove if needed
	//if (auto shader = material->getShaderParams()[shaderAlpha]; isGoodMat)
	//{
	//	!isShutdown
	//		? shader->setValue(config.get<float>(vars.fShaderParam) / 100.0f)
	//		: shader->setValue(1.0f); // default val reset
	//}
	//}
}