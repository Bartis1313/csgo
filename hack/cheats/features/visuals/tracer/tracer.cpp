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

#include "../misc/bulletUpdater.hpp"

void BulletTracer::draw()
{
	if (!vars::visuals->world->tracer->enabled)
		return;

	if (!game::isAvailable())
		return;

	if (!game::localPlayer->isAlive())
		return;

	auto convertToFlag = [](const std::string& flag)
	{
		int ret{ 0 };
		for (const auto& el : std::views::split(flag, '|'))
		{
			const std::string v{ el.begin(), el.end() };
			const int num = std::stoi(v);
			ret |= num;
		}

		return ret;
	};

	const auto lastbullets = g_BulletUpdater->getLastBullets();
	if (lastbullets.empty())
		return;

	CfgBeam cfgbeam = vars::visuals->world->tracer->beamTracer;
	auto& strWithoutSpaces = cfgbeam.flags;
	strWithoutSpaces.erase(std::remove(strWithoutSpaces.begin(), strWithoutSpaces.end(), ' '), strWithoutSpaces.end());
	const Vec3 correctEnd = lastbullets.size() == 2 ? lastbullets.back() : lastbullets.front();
	const Vec3 src{ game::localPlayer->getEyePos() };

	BeamInfo_t info = {};
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
	info.m_vecEnd = correctEnd;
	info.m_segments = cfgbeam.segments;
	info.m_renderable = true;
	
	memory::interfaces::beams->createBeamPoints(info);
}