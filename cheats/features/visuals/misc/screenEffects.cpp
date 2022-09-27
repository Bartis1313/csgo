#include "screenEffects.hpp"

#include <SDK/IMaterialSystem.hpp>
#include <SDK/IMatRenderContext.hpp>
#include <SDK/vars.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <menu/GUI-ImGui/selections.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>

void ScreenEffects::init()
{
	for (const std::string_view el : selections::screenEffects)
	{
		bool mark = false;
		if (el == "effects/nightvision") // this looks bad, dunno
			mark = true;

		m_materials.emplace_back(std::make_pair(interfaces::matSys->findMaterial(
			el.data(), TEXTURE_GROUP_CLIENT_EFFECTS), mark));
	}
}

void ScreenEffects::run()
{
	if (!game::isAvailable())
		return;

	int cfg = config.get<int>(vars.iScreenEffect);
	if (cfg == 0)
		return;

	Color color = config.get<CfgColor>(vars.cScreenEffect).getColor();

	auto material = m_materials.at(cfg).first;
	material->colorModulate(color); // -> works for night vision

	static bool found = false;
	auto var = material->findVar(XOR("$c0_x"), &found);
	if(found)
		var->setValue(config.get<float>(vars.fScreenEffectParam));

	if (m_materials.at(cfg).second) //nightvisioson
		game::localPlayer->m_flNightVisionAlpha() = color.a();

	// now there should be if for motion blur as it needs much more than just drawing this material

	auto ctx = interfaces::matSys->getRenderContext();
	ctx->drawScreenSpaceRectangle(material, 0, 0, globals::screenX, globals::screenY, 0, 0,
	 static_cast<float>(globals::screenX), static_cast<float>(globals::screenY), globals::screenX, globals::screenY);
	ctx->release();
}
