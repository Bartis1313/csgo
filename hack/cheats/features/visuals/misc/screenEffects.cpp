#include "screenEffects.hpp"

#include <SDK/IMaterialSystem.hpp>
#include <SDK/IMatRenderContext.hpp>
#include <SDK/vars.hpp>
#include <SDK/materialInit.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <menu/GUI-ImGui/selections.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>
#include <gamememory/memory.hpp>
#include <utilities/console/console.hpp>

#include <cheats/hooks/screen2dEffect.hpp>

namespace
{
	struct ScreenEffectshandler : hooks::Screen2DEffect
	{
		ScreenEffectshandler()
		{
			this->registerInit(screenEffects::initMaterials);
			this->registerRun(screenEffects::run);
		}
	} screenEffectsHandler;
}

namespace screenEffects
{
	std::vector<std::pair<IMaterial*, bool>> screenMaterials;
}

void screenEffects::initMaterials()
{
	for (const std::string_view el : selections::screenEffects)
	{
		bool mark = false;
		if (el == "effects/nightvision") // nighvision is special case, because overwriting a netvar just works
			mark = true;

		screenMaterials.emplace_back(std::make_pair(memory::interfaces::matSys->findMaterial(
			el.data(), TEXTURE_GROUP_CLIENT_EFFECTS), mark));
	}
}

void screenEffects::run(CViewSetup* view)
{
	if (!game::isAvailable())
		return;

	int cfg = vars::visuals->world->screenEffect->index;
	if (cfg == 0 || globals::isShutdown)
	{
		if (auto& night = game::localPlayer->m_bNightVisionOn())
			night = false;

		return;
	}

	const Color color = vars::visuals->world->screenEffect->color();

	const auto [material, isNightVision] = screenMaterials.at(cfg);
	material->colorModulate(color); // wont work for every material, to do on every refer how glow chams are made

	// only materials in special key have this, not all
	bool found{ false };
	if(const auto var = material->findVar("$c0_x", &found); var && found)
		var->setValue(vars::visuals->world->screenEffect->param);

	if (auto& night = game::localPlayer->m_bNightVisionOn(); !night && isNightVision)
	{
		night = true;
		game::localPlayer->m_flNightVisionAlpha() = color.a();
	}
	else if(night && !isNightVision)
		night = false;

	const auto ctx = memory::interfaces::matSys->getRenderContext();
	ctx->drawScreenEffectMaterial(material);
}
