#include "trails.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/IViewRenderBeams.hpp>
#include <SDK/IEffects.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <render/render.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>

void Trails::draw()
{
	if (!game::isAvailable())
		return;

	const int type = vars::misc->trail->mode;

	if (!vars::misc->trail->enabled)
		return;

	const Color color = vars::misc->trail->color();

	switch (type)
	{
	case E2T(MovementTrail::BEAM):
	{
		if (!game::localPlayer->isMoving())
			return;

		static Vec3 end = game::localPlayer->absOrigin();
		const Vec3 start = game::localPlayer->absOrigin();

		BeamInfo_t info = {};

		info.m_type = TE_BEAMPOINTS;
		info.m_modelName = "sprites/purplelaser1.vmt";
		info.m_modelIndex = -1;
		info.m_vecStart = start;
		info.m_vecEnd = end;
		info.m_haloIndex = -1;
		info.m_haloScale = 0.0f;
		info.m_life = vars::misc->trail->time;
		info.m_width = 5.0f;
		info.m_endWidth = 5.0f;
		info.m_fadeLength = 0.0f;
		info.m_amplitude = 2.0;
		info.m_brightness = 255.0f;
		info.m_red = color.rMultiplied();
		info.m_green = color.gMultiplied();
		info.m_blue = color.bMultiplied();
		info.m_speed = vars::misc->trail->beamSpeed;
		info.m_startFrame = 0;
		info.m_frameRate = 0.0f;
		info.m_segments = 2;
		info.m_renderable = true;

		auto myBeam = memory::interfaces::beams->createBeamPoints(info);

		// change to pos after beam is drawn, since it's static it's possible
		end = start;

		break;
	}
	case E2T(MovementTrail::LINE):
	{
		if (game::localPlayer->isMoving())
			m_trails.emplace_back(game::localPlayer->m_vecOrigin(),
				memory::interfaces::globalVars->m_curtime + vars::misc->trail->time, color);

		std::vector<ImVec2> points;
		for (size_t i = 0; const auto & el : m_trails)
		{
			if (el.m_expire < memory::interfaces::globalVars->m_curtime)
			{
				m_trails.erase(m_trails.begin() + i);
				continue;
			}

			if (ImVec2 screen; imRender.worldToScreen(el.m_pos, screen))
				points.push_back(screen);

			i++;
		}

		if (!points.empty())
			imRender.drawPolyLine(points, color, 0, 2.0f);

		break;
	}
	case E2T(MovementTrail::SPLASH):
	{
		if (game::localPlayer->isMoving())
			memory::interfaces::effects->energySplash(game::localPlayer->m_vecOrigin(),
				 game::localPlayer->m_angEyeAngles() * (game::localPlayer->m_vecVelocity().length() / game::localPlayer->m_flMaxspeed()), true);

		break;
	}
	default:
		break;
	}
}