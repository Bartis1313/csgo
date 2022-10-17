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
#include <SDK/interfaces/interfaces.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/renderer/renderer.hpp>
#include <utilities/utilities.hpp>
#include <menu/GUI-ImGui/selections.hpp>

#include <ranges>


void BulletTracer::init()
{
	g_Events->add(XOR("bullet_impact"), std::bind(&BulletTracer::draw, this, std::placeholders::_1));
}

void BulletTracer::draw(IGameEvent* event)
{
	auto attacker = interfaces::entList->getClientEntity(interfaces::engine->getPlayerID(event->getInt(XOR("userid"))));
	if (!attacker)
		return;

	if (attacker != game::localPlayer)
		return;

	auto local = reinterpret_cast<Player_t*>(attacker);

	float x = event->getFloat(XOR("x"));
	float y = event->getFloat(XOR("y"));
	float z = event->getFloat(XOR("z"));

	Vec3 src = local->getEyePos();
	src[Coord::Z] += 1.0f;
	Vec3 dst = Vec3{ x, y, z };

	if (!vars::visuals->world->tracer->enabled)
		return;

	CfgBeam cfgbeam = vars::visuals->world->tracer->beamTracer;

	Trace_t tr;
	TraceFilter filter;
	filter.m_skip = game::localPlayer();
	interfaces::trace->traceRay({ src, dst }, MASK_PLAYER, &filter, &tr);

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

	auto strWithoutSpaces = cfgbeam.flags;
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

	interfaces::beams->createBeamPoints(info);
}
