#include "hat.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/Input.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/math/math.hpp>
#include <utilities/renderer/renderer.hpp>

void Hat::init()
{

}

void Hat::draw()
{
	if (!config.get<bool>(vars.bHat))
		return;

	if (!game::isAvailable())
		return;

	if (!interfaces::input->m_cameraInThirdPerson)
		return;

	auto pos = game::localPlayer->getBonePos(8); // you can play around with vec.z

	// config.get<> enjoyer

	if (config.get<bool>(vars.bHatRainbow))
	{
		drawConeEditedRainbow(pos, config.get<float>(vars.fHatRadius), 86, config.get<float>(vars.fHatSize), config.get<float>(vars.fHatSpeed), config.get<int>(vars.iHatTriangleAlpha), config.get<int>(vars.iHatLinesAlpha));
	}
	else
	{
		imRender.drawCone(pos, config.get<float>(vars.fHatRadius), 86, config.get<float>(vars.fHatSize),
			config.get<CfgColor>(vars.cHatTriangle).getColor(), config.get<CfgColor>(vars.cHatLine).getColor(), true, 2.0f);
	}
}

void Hat::drawConeEditedRainbow(const Vector& pos, const float radius, const int points, const float size,
	const float speed, const int trianglesAlpha, const int linesAlpha, const float thickness)
{
	ImVec2 orignalW2S = {};
	if (!imRender.worldToScreen(pos, orignalW2S))
		return;

	float step = math::PI * 2.0f / points;
	for (float angle = 0.0f; angle < (math::PI * 2.0f); angle += step)
	{
		Vector startWorld = { radius * std::cos(angle) + pos.x, radius * std::sin(angle) + pos.y, pos.z };
		Vector endWorld = { radius * std::cos(angle + step) + pos.x, radius * std::sin(angle + step) + pos.y, pos.z };

		if (ImVec2 start, end; imRender.worldToScreen(startWorld, start) && imRender.worldToScreen(endWorld, end))
		{
			imRender.drawLine(start, end, Color::rainbowColor(interfaces::globalVars->m_curtime + angle, speed).getColorEditAlphaInt(linesAlpha), thickness);
			imRender.drawTrianglePoly({ orignalW2S.x, orignalW2S.y + size }, start, end, Color::rainbowColor(interfaces::globalVars->m_curtime + angle, speed).getColorEditAlphaInt(trianglesAlpha));
		}
	}
}