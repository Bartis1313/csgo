#include "freeCam.hpp"

#include <SDK/CViewSetup.hpp>
#include <SDK/math/Vector.hpp>
#include <utilities/inputSystem.hpp>
#include <utilities/math/math.hpp>
#include <config/vars.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <menu/GUI-ImGui/menu.hpp>
#include <game/game.hpp>

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
		if (inputHandler.isKeyPressed(VK_SHIFT))
			vars::misc->freeCam->speed += 0.1f;
		else if (inputHandler.isKeyPressed(VK_CONTROL))
			vars::misc->freeCam->speed -= 0.1f;

		Vec3 ang = view->m_angles;
		const float speed = std::clamp(vars::misc->freeCam->speed, 1.0f, 20.0f);

		const auto [forward, right, up] = math::angleVectors(ang);
		if (inputHandler.isKeyDown(VK_SPACE))
		{
			v += up * speed;
		}
		if (inputHandler.isKeyDown(0x57)) // w
		{
			v += forward * speed;
		}
		if (inputHandler.isKeyDown(0x41)) // a
		{
			v -= right * speed;
		}
		if (inputHandler.isKeyDown(0x53)) // s
		{
			v -= forward * speed;
		}
		if (inputHandler.isKeyDown(0x44)) // d
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

#include <dependencies/ImGui/imgui.h>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>

void FreecamDraw::draw()
{
	if (!g_Freecam->isInCam())
		return;

	if (ImGui::Begin(XOR("Info##cam"), nullptr, ImGuiWindowFlags_AlwaysAutoResize /*| ImGuiWindowFlags_NoResize*/ | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::TextUnformatted(FORMAT(XOR("Speed: {:.2f}"), vars::misc->freeCam->speed).c_str());

		ImGui::End();
	}
}