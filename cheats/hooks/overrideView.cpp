#include "hooks.hpp"
#include "../globals.hpp"
#include "../menu/vars.hpp"
#include "../game.hpp"

void __stdcall hooks::overrideView::hooked(CViewSetup* view)
{	
	if(!interfaces::engine->isInGame() || !game::localPlayer)
		return original(view);

	if (!game::localPlayer->m_bIsScoped())
		if(vars::iFOV > 0 || vars::iFOV < 0)
			view->m_fov += vars::iFOV;

	globals::FOV = view->m_fov;

	original(view);
}