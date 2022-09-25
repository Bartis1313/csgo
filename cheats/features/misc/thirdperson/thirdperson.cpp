#include "thirdperson.hpp"

#include <SDK/IEngineTrace.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/Input.hpp>
#include <SDK/vars.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>
#include <utilities/math/math.hpp>
#include <config/vars.hpp>


void Thirdperson::init()
{
	
}

void Thirdperson::run(CViewSetup* view)
{
	if (!config.get<bool>(vars.bThirdp))
		return;

	if (!game::isAvailable())
		return;

	if (bool state = config.get<Key>(vars.kThirdp).isEnabled(); state)
	{
		Vector angles;
		interfaces::engine->getViewAngles(angles);

		Trace_t trace;

		float fixedX = angles.x += config.get<float>(vars.fThirdpX);
		float fixedY = angles.y += config.get<float>(vars.fThirdpY);

		Vector camera =
		{
			std::cos(math::DEG2RAD(fixedY)) * config.get<float>(vars.fThirdpDistance),
			std::sin(math::DEG2RAD(fixedY)) * config.get<float>(vars.fThirdpDistance),
			std::sin(math::DEG2RAD(-fixedX))* config.get<float>(vars.fThirdpDistance),
		};
		Vector eyePos = game::localPlayer->getEyePos();
		TraceFilter filter;
		filter.m_skip = game::localPlayer();
		interfaces::trace->traceRay({ eyePos, (eyePos - camera) }, MASK_SOLID, &filter, &trace);

		interfaces::input->m_cameraInThirdPerson = state;
		interfaces::input->m_cameraOffset = Vector{ fixedX, fixedY, config.get<float>(vars.fThirdpDistance) * ((trace.m_fraction < 1.0f) ? trace.m_fraction : 1.0f) };
	}
	else if (globals::isShutdown || !state)
	{
		interfaces::input->m_cameraInThirdPerson = false;
		interfaces::input->m_cameraOffset = {};
	}
}