#include "hooks.hpp"
#include "../globals.hpp"
#include "../menu/vars.hpp"
#include "../game.hpp"

void __stdcall hooks::overrideView::hooked(CViewSetup* view)
{	
	if(!interfaces::engine->isInGame() || !game::localPlayer)
		return original(view);

	globals::FOV = view->m_fov;

	if (!game::localPlayer->m_bIsScoped())
		if(vars::iFOV > 0 || vars::iFOV < 0)
			view->m_fov += vars::iFOV;

	original(view);
}