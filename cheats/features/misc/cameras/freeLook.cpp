#include "freeLook.hpp"

#include <SDK/CUserCmd.hpp>
#include <SDK/CViewSetup.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <config/vars.hpp>

void FreeLook::run(CUserCmd* cmd)
{
	if (!vars::misc->freeLook->enabled)
		return;

	if (!vars::keys->freeLook.isEnabled())
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

void FreeLookViewer::run(CViewSetup* view)
{
	if (!vars::misc->freeLook->enabled)
		return;

	if (vars::keys->freeLook.isEnabled())
	{
		const auto look = g_FreeLook->m_lastAngleLook;

		if (look.isZero())
			return;

		view->m_angles[0] = look[0];
		view->m_angles[1] = look[1];
	}
}