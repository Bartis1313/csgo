#include "hooks.hpp"

#include "../classes/overrideView.hpp"

#include <SDK/CViewSetup.hpp>
#include <SDK/structs/Entity.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <game/globals.hpp>
#include <game/game.hpp>
#include <config/vars.hpp>

void __stdcall hooks::overrideView::hooked(CViewSetup* view)
{	
	if(!game::isAvailable())
		return original(view);

	if (!game::localPlayer->m_bIsScoped())
		if (auto fov = config.get<float>(vars.fFOV); fov > 0.0f || fov < 0.0f)
			view->m_fov += fov;

	globals::FOV = view->m_fov;
	OverrideViewType::runAll(view);

	original(view);
}