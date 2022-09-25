#include "freeCam.hpp"

#include <SDK/CViewSetup.hpp>
#include <SDK/math/Vector.hpp>
#include <utilities/inputSystem.hpp>
#include <utilities/math/math.hpp>
#include <config/vars.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <menu/GUI-ImGui/menu.hpp>

#include <game/game.hpp>

void Freecam::init()
{

}

void Freecam::run(CViewSetup* view)
{
	if (menu.isMenuActive())
		return;

	if (!game::isAvailable())
		return;

	if (!config.get<bool>(vars.bFreeCam))
	{
		m_inCam = false;
		return;
	}

	// OR get center from map info?
	static Vector v = view->m_angles;

	if (config.get<Key>(vars.kFreeCam).isEnabled())
	{
		m_inCam = true;

		float& speed = config.getRef<float>(vars.fFreeCam);
		Vector ang = view->m_angles;

		float sinYaw = std::sin(math::DEG2RAD(ang.y));
		float sinPitch = std::sin(math::DEG2RAD(ang.x));
		float cosYaw = std::cos(math::DEG2RAD(ang.y));
		float cosPitch = std::cos(math::DEG2RAD(ang.x));

		// to make this ideal we also can do cases like W+A
		// pseudo: correct.x = (-sin(view.x) + cos(view.x)) / 2.0f; ...something like this

		if (inputHandler.isKeyPressed(VK_SHIFT))
			speed += 0.1f;
		else if (inputHandler.isKeyPressed(VK_CONTROL))
			speed -= 0.1f;

		speed = std::clamp(speed, 1.0f, 20.0f);

		if (inputHandler.isKeyDown(VK_SPACE))
		{
			v.x += cosYaw * cosPitch;
			v.y += sinYaw * cosPitch;
			v.z += std::sin(math::DEG2RAD(-(ang.x - 90.0f)));
		}
		if(inputHandler.isKeyDown(0x57)) // w
		{
			v.x += cosYaw * cosPitch;
			v.y += sinYaw * cosPitch;
			v.z += -sinPitch;
		}
		if (inputHandler.isKeyDown(0x41)) // a
		{
			v.x += std::cos(math::DEG2RAD(ang.y + 90.0f)) * cosPitch;
			v.y += std::sin(math::DEG2RAD(ang.y + 90.0f)) * cosPitch;
			v.z += -sinPitch;
		}
		if (inputHandler.isKeyDown(0x53)) // s
		{
			v.x -= cosYaw * cosPitch;
			v.y -= sinYaw * cosPitch;
			v.z -= -sinPitch;
		}
		if (inputHandler.isKeyDown(0x44)) // d
		{
			v.x -= std::cos(math::DEG2RAD(ang.y + 90.0f)) * cosPitch;
			v.y -= std::sin(math::DEG2RAD(ang.y + 90.0f)) * cosPitch;
			v.z += -sinPitch;
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

void FreecamDraw::init()
{

}

void FreecamDraw::draw()
{
	if (!g_Freecam.isInCam())
		return;

	if (ImGui::Begin(XOR("Info##cam"), nullptr, ImGuiWindowFlags_AlwaysAutoResize /*| ImGuiWindowFlags_NoResize*/ | ImGuiWindowFlags_NoCollapse))
	{
		ImGui::TextUnformatted(FORMAT(XOR("Speed: {:.2f}"), config.get<float>(vars.fFreeCam)).c_str());

		ImGui::End();
	}
}