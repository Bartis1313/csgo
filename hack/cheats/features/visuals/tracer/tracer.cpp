#include "tracer.hpp"

#include "../../events/events.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/IGameEvent.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/IEngineTrace.hpp>
#include <SDK/IViewRenderBeams.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/clientHitVerify.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <render/render.hpp>
#include <utilities/utilities.hpp>
#include <menu/GUI-ImGui/selections.hpp>

#include <ranges>

void BulletTracer::draw()
{
	if (!vars::visuals->world->tracer->enabled)
		return;

	if (!game::isAvailable())
		return;

	if (!game::localPlayer->isAlive())
		return;

	CfgBeam cfgbeam = vars::visuals->world->tracer->beamTracer;

	auto m_vecBulletVerifyListClient = game::localPlayer->m_vecBulletVerifyListClient();
	static int gameBulletCount = m_vecBulletVerifyListClient.m_size; // init current count

	for (int i = m_vecBulletVerifyListClient.m_size; i > gameBulletCount; i--) // get current bullets, NOT all
		m_hitsClientSide.emplace_back(HitStruct_t
			{
				m_vecBulletVerifyListClient[i - 1].m_pos,
				memory::interfaces::globalVars->m_curtime + cfgbeam.life
			});

	if (m_vecBulletVerifyListClient.m_size != gameBulletCount)
		gameBulletCount = m_vecBulletVerifyListClient.m_size;

	Color outline = vars::visuals->world->impacts->colorClient();
	Color fill = vars::visuals->world->impacts->colorClientFill();

	for (size_t i = 0; auto & el : m_hitsClientSide)
	{
		float diff = el.m_expire - memory::interfaces::globalVars->m_curtime;

		if (diff < 0.0f)
		{
			m_hitsClientSide.erase(m_hitsClientSide.begin() + i);
			continue;
		}

		if (el.called)
			continue;

		if (!el.called)
		{
			Trace_t tr;
			Vec3 src = game::localPlayer->getEyePos();
			TraceFilter filter;
			filter.m_skip = game::localPlayer();
			memory::interfaces::trace->traceRay({ src, el.m_pos }, MASK_PLAYER, &filter, &tr);

			auto convertToFlag = [](const std::string& flag)
			{
				int ret = 0;
				for (const auto& el : std::views::split(flag, '|'))
				{
					std::string v{ el.begin(), el.end() };
					int num = std::stoi(v);
					ret |= num;
				}

				return ret;
			};

			BeamInfo_t info = {};

			auto& strWithoutSpaces = cfgbeam.flags;
			strWithoutSpaces.erase(std::remove(strWithoutSpaces.begin(), strWithoutSpaces.end(), ' '), strWithoutSpaces.end());

			info.m_type = /*convertToFlag(config.get<std::string>(vars.sBulletTracerType))*/ TE_BEAMPOINTS;
			info.m_flags = convertToFlag(strWithoutSpaces);
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
			info.m_vecStart = src;
			info.m_vecEnd = tr.m_end;
			info.m_segments = cfgbeam.segments;
			info.m_renderable = true;

			memory::interfaces::beams->createBeamPoints(info);

			el.called = true;
		}

		i++;
	}
}