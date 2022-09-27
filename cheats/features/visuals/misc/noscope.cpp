#include "noscope.hpp"

#include <SDK/IMaterialSystem.hpp>
#include <SDK/vars.hpp>
#include <SDK/Enums.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/renderer/renderer.hpp>

void NoScope::init()
{

}

void NoScope::draw()
{
	if (!config.get<bool>(vars.bNoScope))
		return;

	if (!game::isAvailable())
		return;

	auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	if (!weapon->isSniper())
		return;

	if (game::localPlayer->m_bIsScoped())
	{
		imRender.drawLine(globals::screenX / 2.0f, 0.0f, globals::screenX / 2.0f, static_cast<float>(globals::screenY), Colors::Black);
		imRender.drawLine(0.0f, globals::screenY / 2.0f, static_cast<float>(globals::screenX), globals::screenY / 2.0f, Colors::Black);
	}
}

void NoScopeBlur::init()
{
	m_blurScope = interfaces::matSys->findMaterial(XOR("dev/scope_bluroverlay"), XOR(TEXTURE_GROUP_OTHER));
}

void NoScopeBlur::run()
{
	m_blurScope->setMaterialVarFlag(MATERIAL_VAR_NO_DRAW, config.get<bool>(vars.bNoScope));
}
