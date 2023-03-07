#include "freeCam.hpp"

#include <SDK/CViewSetup.hpp>
#include <SDK/math/Vector.hpp>
#include <utilities/inputSystem.hpp>
#include <utilities/math/math.hpp>
#include <config/vars.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <menu/GUI-ImGui/menu.hpp>
#include <cheats/game/game.hpp>

void Freecam::updateKeys()
{
	vars::keys->freeCam.update();
}

void Freecam::run(CViewSetup* view)
{
	if (menu.isMenuActive())
		return;

	if (!game::isAvailable())
		return;

	static Vec3 v = view->m_angles;

	if (vars::misc->freeCam->enabled && vars::keys->freeCam.isEnabled())
	{
		m_inCam = true;
		if (KeysHandler::isKeyPressed(VK_SHIFT))
			vars::misc->freeCam->speed += 0.1f;
		else if (KeysHandler::isKeyPressed(VK_CONTROL))
			vars::misc->freeCam->speed -= 0.1f;

		Vec3 ang = view->m_angles;
		const float speed = std::clamp(vars::misc->freeCam->speed, 1.0f, 20.0f);

		const auto [forward, right, up] = math::angleVectors(ang);
		if (KeysHandler::isKeyDown(VK_SPACE))
		{
			v += up * speed;
		}
		if (KeysHandler::isKeyDown(0x57)) // w
		{
			v += forward * speed;
		}
		if (KeysHandler::isKeyDown(0x41)) // a
		{
			v -= right * speed;
		}
		if (KeysHandler::isKeyDown(0x53)) // s
		{
			v -= forward * speed;
		}
		if (KeysHandler::isKeyDown(0x44)) // d
		{
			v += right * speed;
		}
		view->m_origin += v;
	}
	else
	{
		v = view->m_angles;
		m_inCam = false;
	}
}

#include <imgui.h>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>

void FreecamDraw::draw()
{
	if (!g_Freecam->isInCam())
		return;

	if (ImGui::Begin("Info##cam", nullptr, ImGuiWindowFlags_AlwaysAutoResize /*| ImGuiWindowFlags_NoResize*/ | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::TextUnformatted(std::format("Speed: {:.2f}", vars::misc->freeCam->speed).c_str());

		ImGui::End();
	}
}