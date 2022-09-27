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
	g_Events.add(XOR("bullet_impact"), std::bind(&BulletTracer::draw, this, std::placeholders::_1));
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

	Vector src = local->getEyePos();
	src.z += 1.0f;
	Vector dst = { x, y, z };

	if (!config.get<bool>(vars.bBulletTracer))
		return;

	CfgColor color = config.get<CfgColor>(vars.cBulletTracer);

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

	auto strWithoutSpaces = config.get<std::string>(vars.sBulletTracer);
	strWithoutSpaces.erase(std::remove(strWithoutSpaces.begin(), strWithoutSpaces.end(), ' '), strWithoutSpaces.end());

	info.m_type = /*convertToFlag(config.get<std::string>(vars.sBulletTracerType))*/ TE_BEAMPOINTS;
	info.m_flags = convertToFlag(strWithoutSpaces);
	info.m_modelName = selections::beamNames.at(config.get<int>(vars.iBulletTracer));
	info.m_modelIndex = -1;
	info.m_haloIndex = -1;
	info.m_haloScale = 0.0f;
	info.m_life = config.get<float>(vars.fBulletTracerLife);
	info.m_width = config.get<float>(vars.fBulletTracerWidth);
	info.m_endWidth = config.get<float>(vars.fBulletTracerWidth);
	info.m_fadeLength = config.get<float>(vars.fBulletTracerFadeLength);
	info.m_amplitude = config.get<float>(vars.fBulletTracerAmplitude);
	info.m_red = color.getColor().rMultiplied();
	info.m_green = color.getColor().gMultiplied();
	info.m_blue = color.getColor().bMultiplied();
	info.m_brightness = color.getColor().aMultiplied();
	info.m_speed = config.get<float>(vars.fBulletTracerSpeed);
	info.m_startFrame = static_cast<int>(config.get<float>(vars.fBulletTracerStartFrame));
	info.m_frameRate = config.get<float>(vars.fBulletTracerFrameRate);
	info.m_vecStart = src;
	info.m_vecEnd = tr.m_end;
	info.m_segments = config.get<int>(vars.fBulletTracerSegments);
	info.m_renderable = true;

	interfaces::beams->createBeamPoints(info);
}
