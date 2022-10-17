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

	if (!vars::misc->freeCam->enabled)
	{
		m_inCam = false;
		return;
	}

	// OR get center from map info?
	static Vec3 v = view->m_angles;

	if (vars::keys->freeCam.isEnabled())
	{
		m_inCam = true;

		float speed = vars::misc->freeCam->speed;
		Vec3 ang = view->m_angles;

		float sinYaw = std::sin(math::DEG2RAD(ang[1]));
		float sinPitch = std::sin(math::DEG2RAD(ang[0]));
		float cosYaw = std::cos(math::DEG2RAD(ang[1]));
		float cosPitch = std::cos(math::DEG2RAD(ang[0]));

		// to make this ideal we also can do cases like W+A
		// pseudo: correct.x = (-sin(view.x) + cos(view.x)) / 2.0f; ...something like this

		if (inputHandler.isKeyPressed(VK_SHIFT))
			speed += 0.1f;
		else if (inputHandler.isKeyPressed(VK_CONTROL))
			speed -= 0.1f;

		speed = std::clamp(speed, 1.0f, 20.0f);

		if (inputHandler.isKeyDown(VK_SPACE))
		{
			v[0] += cosYaw * cosPitch;
			v[1] += sinYaw * cosPitch;
			v[2] += std::sin(math::DEG2RAD(-(ang[0] - 90.0f)));
		}
		if(inputHandler.isKeyDown(0x57)) // w
		{
			v[0] += cosYaw * cosPitch;
			v[1] += sinYaw * cosPitch;
			v[2] += -sinPitch;
		}
		if (inputHandler.isKeyDown(0x41)) // a
		{
			v[0] += std::cos(math::DEG2RAD(ang[1] + 90.0f)) * cosPitch;
			v[1] += std::sin(math::DEG2RAD(ang[1] + 90.0f)) * cosPitch;
			v[2] += -sinPitch;
		}
		if (inputHandler.isKeyDown(0x53)) // s
		{
			v[0] -= cosYaw * cosPitch;
			v[1] -= sinYaw * cosPitch;
			v[2] -= -sinPitch;
		}
		if (inputHandler.isKeyDown(0x44)) // d
		{
			v[0] -= std::cos(math::DEG2RAD(ang[1] + 90.0f)) * cosPitch;
			v[1] -= std::sin(math::DEG2RAD(ang[1] + 90.0f)) * cosPitch;
			v[2] += -sinPitch;
		}
		view->m_origin = v * speed;
	}
	// reset should be done by some key or button
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