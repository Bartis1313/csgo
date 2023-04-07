#include "hooks.hpp"

#include <config/vars.hpp>
#include <cheats/game/game.hpp>
#include <SDK/structs/Entity.hpp>

// or hook CHudElement::ShouldDraw() and do this here, and set the field inside CHudElement stackptr
hooks::chudIsHidden::value FASTCALL hooks::chudIsHidden::hooked(FAST_ARGS, int flags)
{
	if (vars::misc->radar->enabled && game::isAvailable())
	{
		// m_bHud_RadarHidden forcing didn't work
		game::localPlayer->m_iHideHUD() |= HIDEHUD_RADAR;
	}
	
	// return true if all elements should be disabled
	return original(thisptr, flags);
}