#include "trails.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/IViewRenderBeams.hpp>
#include <SDK/IEffects.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/renderer/renderer.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>

void Trails::init()
{

}

void Trails::draw()
{
	static Vec3 end;

	if (!game::isAvailable())
		return;

	int type = vars::misc->trail->mode;

	if (!vars::misc->trail->enabled)
		return;

	if (type != E2T(MovementTrail::BEAM))
	{
		// prepare the point to be corrected
		end = game::localPlayer->m_vecOrigin();
	}

	Color color = vars::misc->trail->color();

	switch (type)
	{
	case E2T(MovementTrail::BEAM):
	{
		if (!game::localPlayer->isMoving()) // do not add beams on not moving
			return;

		const Vec3 start = game::localPlayer->m_vecOrigin();

		BeamInfo_t info = {};

		info.m_type = TE_BEAMPOINTS;
		info.m_modelName = XOR("sprites/purplelaser1.vmt");
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

		auto myBeam = interfaces::beams->createBeamPoints(info);

		// change to pos after beam is drawn, since it's static it's possible
		end = start;

		break;
	}
	case E2T(MovementTrail::LINE):
	{
		float curtime = interfaces::globalVars->m_curtime;

		if (game::localPlayer->isMoving())
			m_trails.push_back(Trail_t{ game::localPlayer->m_vecOrigin(), curtime + vars::misc->trail->time, color });

		Vec3 last = {};
		if (!m_trails.empty())
			last = m_trails.front().m_pos;

		for (size_t i = 0; const auto & el : m_trails)
		{
			if (el.m_expire < curtime)
			{
				m_trails.erase(m_trails.begin() + i);
				continue;
			}

			if (ImVec2 start, end; !last.isZero() && imRender.worldToScreen(el.m_pos, start) && imRender.worldToScreen(last, end))
				imRender.drawLine(start, end, el.m_col, 3.0f);

			last = el.m_pos;

			i++;
		}

		break;
	}
	case E2T(MovementTrail::SPLASH):
	{
		if (game::localPlayer->isMoving())
			interfaces::effects->energySplash(game::localPlayer->m_vecOrigin(), {}, true);

		break;
	}
	default:
		break;
	}
}