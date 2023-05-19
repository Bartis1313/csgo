#include "freeCam.hpp"

#include <SDK/CViewSetup.hpp>
#include <SDK/math/Vector.hpp>
#include <utilities/inputSystem.hpp>
#include <utilities/math/math.hpp>
#include <config/vars.hpp>
#include <SDK/IVModelInfo.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <menu/GUI-ImGui/menu.hpp>
#include <cheats/game/game.hpp>

#include <cheats/hooks/overrideView.hpp>
#include <cheats/hooks/wndproc.hpp>

namespace
{
	struct FreecamKeys : hooks::wndProcSys
	{
		FreecamKeys()
		{
			this->registerRun(freecam::updateKeys);
		}
	} freecamKeys;

	struct FreecamView : hooks::OverrideView
	{
		FreecamView()
		{
			this->registerRun(freecam::run);
		}
	} freecamView;
}

void freecam::updateKeys()
{
	vars::keys->freeCam.update();
}

void freecam::run(CViewSetup* view)
{
	if (!game::isAvailable())
		return;

	static Vec3 viewAngles{ view->m_angles };

	if (vars::misc->freeCam->enabled && vars::keys->freeCam.isEnabled())
	{
		isInCam = true;

		/*auto model = game::localPlayer->getModel();
		if (!model)
			return;

		auto studio = memory::interfaces::modelInfo->getStudioModel(model);
		if (!studio)
			return;

		studio->m_flags |= 0x0080;*/

		const auto [forward, right, up] = math::angleVectors(view->m_angles);

		if (inputSystem::isKeyDown(VK_SPACE))
		{
			viewAngles += up * vars::misc->freeCam->speed;
		}
		if (inputSystem::isKeyDown(0x57)) // w
		{
			viewAngles += forward * vars::misc->freeCam->speed;
		}
		if (inputSystem::isKeyDown(0x41)) // a
		{
			viewAngles -= right * vars::misc->freeCam->speed;
		}
		if (inputSystem::isKeyDown(0x53)) // s
		{
			viewAngles -= forward * vars::misc->freeCam->speed;
		}
		if (inputSystem::isKeyDown(0x44)) // d
		{
			viewAngles += right * vars::misc->freeCam->speed;
		}

		view->m_origin += viewAngles;
	}
	else
	{
		viewAngles = view->m_angles;
		isInCam = false;
	}
}