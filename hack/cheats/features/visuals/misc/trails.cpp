#include "trails.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/IViewRenderBeams.hpp>
#include <SDK/IEffects.hpp>
#include <SDK/material.hpp>
#include <SDK/IMaterialSystem.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <render/render.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <menu/GUI-ImGui/selections.hpp>

#include <cheats/hooks/paintTraverse.hpp>

namespace
{
	struct TrailsHandler : hooks::PaintTraverse
	{
		TrailsHandler()
		{
			this->registerRender(trails::draw);
		}
	} trailsHandler;
}

namespace trails
{
	struct Trail_t
	{
		Vec3 pos;
		float expire;
	};

	std::vector<Trail_t> trailsList;
}

void trails::draw()
{
	if (!game::isAvailable())
		return;

	const int type = vars::misc->trail->mode;

	if (!vars::misc->trail->enabled)
		return;

	switch (type)
	{
	case E2T(MovementTrail::BEAM):
	{
		if (!game::localPlayer->isMoving())
			return;
		
		// we will use vecorigin for trails, absorigin updates too often
		static Vec3 end = game::localPlayer->m_vecOrigin();
		const Vec3 start = game::localPlayer->m_vecOrigin();

		CfgBeam cfgbeam = vars::misc->trail->beam;

		BeamInfo_t info = {};

		// or create a beam follow, I ended up displaying nothing
		info.m_type = TE_BEAMPOINTS;
		info.m_modelName = selections::beamNames.at(cfgbeam.index);
		info.m_modelIndex = -1;
		info.m_haloIndex = -1;
		info.m_haloScale = 0.0f;
		info.m_life = cfgbeam.life;
		info.m_width = cfgbeam.width;
		info.m_endWidth = cfgbeam.width;
		info.m_fadeLength = cfgbeam.fadeLength;
		info.m_amplitude = cfgbeam.amplitude;
		info.m_red = cfgbeam.color().rMultiplied();
		info.m_green = cfgbeam.color().gMultiplied();
		info.m_blue = cfgbeam.color().bMultiplied();
		info.m_brightness = cfgbeam.color().aMultiplied();
		info.m_speed = cfgbeam.speed;
		info.m_startFrame = static_cast<int>(cfgbeam.startFrame);
		info.m_frameRate = cfgbeam.frameRate;
		info.m_vecStart = start;
		info.m_vecEnd = end;
		info.m_segments = cfgbeam.segments;
		info.m_renderable = true;

		memory::interfaces::beams->createBeamPoints(info);

		end = start;

		break;
	}
	case E2T(MovementTrail::LINE):
	{
		if (game::localPlayer->isMoving())
			trailsList.emplace_back(game::localPlayer->m_vecOrigin(),
				memory::interfaces::globalVars->m_curtime + vars::misc->trail->lineLife);

		std::vector<ImVec2> points;
		for (size_t i = 0; const auto [pos, expire] : trailsList)
		{
			if (expire < memory::interfaces::globalVars->m_curtime)
			{
				trailsList.erase(trailsList.begin() + i);
				continue;
			}

			if (ImVec2 screen; ImRender::worldToScreen(pos, screen))
				points.push_back(screen);

			i++;
		}

		if (!points.empty())
			ImRender::drawPolyLine(points, vars::misc->trail->colorLine(), 0, 2.0f);

		break;
	}
	case E2T(MovementTrail::SPLASH):
	{
		static auto spark = memory::interfaces::matSys->findMaterial("effects/spark", TEXTURE_GROUP_PARTICLE);
		if (spark)
			spark->modulateAllColor(vars::misc->trail->colorSplash());

		if (game::localPlayer->isMoving())
			memory::interfaces::effects->energySplash(game::localPlayer->m_vecOrigin(),
				 game::localPlayer->m_angEyeAngles() * (game::localPlayer->m_vecVelocity().length() / game::localPlayer->m_flMaxspeed()), true);

		break;
	}
	default:
		break;
	}
}