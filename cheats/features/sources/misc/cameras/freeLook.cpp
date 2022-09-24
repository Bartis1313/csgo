#include "freeLook.hpp"

#include <SDK/CUserCmd.hpp>
#include <SDK/CViewSetup.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <config/vars.hpp>

void FreeLook::init()
{

}

void FreeLook::run(CUserCmd* cmd)
{
	if (!config.get<bool>(vars.bFreeLook))
		return;

	if (!config.get<Key>(vars.kFreeLook).isEnabled())
	{
		m_lastAngle = cmd->m_viewangles;
		m_lastAngleLook = {};
	}
	else
	{
		if (m_lastAngleLook.isZero())
			m_lastAngleLook = m_lastAngle;

		m_lastAngleLook += (cmd->m_viewangles - m_lastAngle);
		m_lastAngleLook.normalize().clamp();

		interfaces::engine->setViewAngles(m_lastAngle);
	}
}

void FreeLookViewer::init()
{

}

void FreeLookViewer::run(CViewSetup* view)
{
	if (!config.get<bool>(vars.bFreeLook))
		return;

	if (config.get<Key>(vars.kFreeLook).isEnabled())
	{
		const auto look = g_FreeLook.m_lastAngleLook;

		if (look.isZero())
			return;

		view->m_angles.x = look.x;
		view->m_angles.y = look.y;
	}
}