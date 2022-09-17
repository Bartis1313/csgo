#include "noscope.hpp"

#include "../../../../../SDK/IMaterialSystem.hpp"
#include "../../../../../SDK/vars.hpp"
#include "../../../../../SDK/Enums.hpp"
#include "../../../../../SDK/interfaces/interfaces.hpp"

#include "../../../../game.hpp"
#include "../../../../globals.hpp"
#include "../../../../../config/vars.hpp"
#include "../../../../../utilities/tools/tools.hpp"
#include "../../../../../utilities/renderer/renderer.hpp"

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
		int x = globals::screenX;
		int y = globals::screenY;
		imRender.drawLine(x / 2.0f, 0.0f, x / 2.0f, y, Colors::Black);
		imRender.drawLine(0.0f, y / 2.0f, x, y / 2.0f, Colors::Black);
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
