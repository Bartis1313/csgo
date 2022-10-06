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
	if (!vars::misc->thirdp->enabled)
		return;

	if (!game::isAvailable())
		return;

	if (bool state = vars::keys->thirdP.isEnabled(); state)
	{
		Vec3 angles = game::getViewAngles();

		Trace_t trace;

		float fixedX = angles[0] += vars::misc->thirdp->x;
		float fixedY = angles[1] += vars::misc->thirdp->y;

		Vec3 camera = Vec3
		{
			std::cos(math::DEG2RAD(fixedY)) * vars::misc->thirdp->distance,
			std::sin(math::DEG2RAD(fixedY)) * vars::misc->thirdp->distance,
			std::sin(math::DEG2RAD(-fixedX))* vars::misc->thirdp->distance,
		};
		Vec3 eyePos = game::localPlayer->getEyePos();
		TraceFilter filter;
		filter.m_skip = game::localPlayer();
		interfaces::trace->traceRay({ eyePos, (eyePos - camera) }, MASK_SOLID, &filter, &trace);

		interfaces::input->m_cameraInThirdPerson = state;
		interfaces::input->m_cameraOffset = Vec3{ fixedX, fixedY, vars::misc->thirdp->distance * ((trace.m_fraction < 1.0f) ? trace.m_fraction : 1.0f) };
	}
	else if (globals::isShutdown || !state)
	{
		interfaces::input->m_cameraInThirdPerson = false;
		interfaces::input->m_cameraOffset = {};
	}
}