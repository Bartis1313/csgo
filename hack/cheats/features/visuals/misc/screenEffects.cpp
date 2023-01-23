#include "screenEffects.hpp"

#include <SDK/IMaterialSystem.hpp>
#include <SDK/IMatRenderContext.hpp>
#include <SDK/vars.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <menu/GUI-ImGui/selections.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>

void ScreenEffects::init()
{
	for (const std::string_view el : selections::screenEffects)
	{
		bool mark = false;
		if (el == "effects/nightvision") // this looks bad, dunno
			mark = true;

		m_materials.emplace_back(std::make_pair(memory::interfaces::matSys->findMaterial(
			el.data(), TEXTURE_GROUP_CLIENT_EFFECTS), mark));
	}
}

void ScreenEffects::run()
{
	if (!game::isAvailable())
		return;

	int cfg = vars::visuals->world->screenEffect->index;
	if (cfg == 0)
		return;

	Color color = vars::visuals->world->screenEffect->color();

	auto material = m_materials.at(cfg).first;
	material->colorModulate(color); // -> works for night vision

	static bool found = false;
	auto var = material->findVar(XOR("$c0_x"), &found);
	if(found)
		var->setValue(vars::visuals->world->screenEffect->param);

	if (m_materials.at(cfg).second) //nightvisioson
		game::localPlayer->m_flNightVisionAlpha() = color.a();

	auto ctx = memory::interfaces::matSys->getRenderContext();
	ctx->drawScreenSpaceRectangle(material, 0, 0, globals::screenX, globals::screenY, 0, 0,
	 static_cast<float>(globals::screenX), static_cast<float>(globals::screenY), globals::screenX, globals::screenY);
	ctx->release();
}
