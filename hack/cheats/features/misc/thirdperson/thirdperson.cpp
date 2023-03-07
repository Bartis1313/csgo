#include "thirdperson.hpp"

#include <SDK/IEngineTrace.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/Input.hpp>
#include <SDK/vars.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <utilities/math/math.hpp>
#include <config/vars.hpp>

void Thirdperson::updateKeys()
{
	vars::keys->thirdP.update();
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
			std::sin(math::DEG2RAD(-fixedX)) * vars::misc->thirdp->distance,
		};
		Vec3 eyePos = game::localPlayer->getEyePos();
		TraceFilter filter;
		filter.m_skip = game::localPlayer();
		memory::interfaces::trace->traceRay({ eyePos, (eyePos - camera) }, MASK_SOLID, &filter, &trace);

		memory::interfaces::input->m_cameraInThirdPerson = state;
		memory::interfaces::input->m_cameraOffset = Vec3{ fixedX, fixedY, vars::misc->thirdp->distance * ((trace.m_fraction < 1.0f) ? trace.m_fraction : 1.0f) };
	}
	else if (globals::isShutdown || !state)
	{
		memory::interfaces::input->m_cameraInThirdPerson = false;
		memory::interfaces::input->m_cameraOffset = {};
	}
}