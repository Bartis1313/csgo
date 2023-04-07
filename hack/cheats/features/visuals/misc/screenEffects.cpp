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

void ScreenEffects::initMaterials()
{
	for (const std::string_view el : selections::screenEffects)
	{
		bool mark = false;
		if (el == "effects/nightvision") // nighvision is special case, because overwriting a netvar just works
			mark = true;

		m_materials.emplace_back(std::make_pair(memory::interfaces::matSys->findMaterial(
			el.data(), TEXTURE_GROUP_CLIENT_EFFECTS), mark));
	}
}

void ScreenEffects::run()
{
	INIT_MATERIALS_ONCE(initMaterials);

	if (!game::isAvailable())
		return;

	int cfg = vars::visuals->world->screenEffect->index;
	if (cfg == 0)
	{
		if (auto& night = game::localPlayer->m_bNightVisionOn())
			night = false;

		return;
	}

	const Color color = vars::visuals->world->screenEffect->color();

	const auto [material, isNightVision] = m_materials.at(cfg);
	material->colorModulate(color); // wont work for every material, to do on every refer how glow chams are made

	// only materials in special key have this, not all
	if(const auto var = material->findVar("$c0_x"))
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
