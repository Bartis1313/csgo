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

void Hat::draw()
{
	if (!vars::misc->hat->enabled)
		return;

	if (!game::isAvailable())
		return;

	if (!interfaces::input->m_cameraInThirdPerson)
		return;

	auto pos = game::localPlayer->getBonePos(8); // you can play around with vec.z

	// config.get<> enjoyer

	if (vars::misc->hat->rainbow)
	{
		drawConeEditedRainbow(pos, vars::misc->hat->radius, 86, vars::misc->hat->size,
			vars::misc->hat->rainbowSpeed, vars::misc->hat->rainbowAlpha, vars::misc->hat->rainbowLinesAlpha);
	}
	else
	{
		imRender.drawCone(pos, vars::misc->hat->radius, 86, vars::misc->hat->size,
			vars::misc->hat->colorTriangle(), vars::misc->hat->colorLine(), true, 2.0f);
	}
}

void Hat::drawConeEditedRainbow(const Vec3& pos, const float radius, const int points, const float size,
	const float speed, const int trianglesAlpha, const int linesAlpha, const float thickness)
{
	ImVec2 orignalW2S = {};
	if (!imRender.worldToScreen(pos, orignalW2S))
		return;

	float step = math::PI * 2.0f / points;
	for (float angle = 0.0f; angle < (math::PI * 2.0f); angle += step)
	{
		Vec3 startWorld = Vec3{ radius * std::cos(angle) + pos[Coord::X], radius * std::sin(angle) + pos[Coord::Y], pos[Coord::Z] };
		Vec3 endWorld = Vec3{ radius * std::cos(angle + step) + pos[Coord::X], radius * std::sin(angle + step) + pos[Coord::Y], pos[Coord::Z] };

		if (ImVec2 start, end; imRender.worldToScreen(startWorld, start) && imRender.worldToScreen(endWorld, end))
		{
			imRender.drawLine(start, end,
				Color::rainbowColor(interfaces::globalVars->m_curtime + angle, speed).getColorEditAlphaInt(linesAlpha), thickness);
			imRender.drawTrianglePoly({ orignalW2S.x, orignalW2S.y + size }, start, end,
				Color::rainbowColor(interfaces::globalVars->m_curtime + angle, speed).getColorEditAlphaInt(trianglesAlpha));
		}
	}
}