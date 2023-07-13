#include "modulateColors.hpp"

#include <SDK/IMaterialSystem.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>
#include <cheats/game/globals.hpp>

#include <array>

struct modulate_t
{
	bool* enabled;
	std::string_view name;
	CfgColor* color;
};

void modulateColor::run(void* thisptr, float* r, float* g, float* b)
{
	const auto material = reinterpret_cast<IMaterial*>(thisptr);

	if (!material)
		return;

	if (material->isError())
		return;

	static std::array mats =
	{
		modulate_t{ &vars::visuals->world->modulate->enabledTexture, TEXTURE_GROUP_WORLD, &vars::visuals->world->modulate->texture },
		modulate_t{ &vars::visuals->world->modulate->enabledProp, TEXTURE_GROUP_STATIC_PROP, &vars::visuals->world->modulate->prop },
		modulate_t{ &vars::visuals->world->modulate->enabledSky, TEXTURE_GROUP_SKYBOX, &vars::visuals->world->modulate->sky },
	};

	bool goodMat = false;
	const std::string_view group = material->getTextureGroupName();
	if (auto itr = std::ranges::find_if(mats, [group](const auto& mat)
		{
			if (!(*mat.enabled))
				return false;

			return mat.name == group;
		}); itr != mats.end())
	{
		itr->color->refresh();
		*r = itr->color->getColor().r();
		*g = itr->color->getColor().g();
		*b = itr->color->getColor().b();

		goodMat = true;
	}

	constexpr int shaderAlpha = 5;
	auto shader = material->getShaderParams()[shaderAlpha];
	if (vars::visuals->world->modulate->enabledShader)
		shader->setValue(vars::visuals->world->modulate->shader / 100.0f);

	if (globals::isShutdown)
		shader->setValue(1.0f);
}