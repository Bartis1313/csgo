#include "freeLook.hpp"

#include "../../../SDK/CUserCmd.hpp"
#include "../../../SDK/CViewSetup.hpp"
#include "../../../SDK/IVEngineClient.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"

#include "../../../config/vars.hpp"

void FreeLook::createMove(CUserCmd* cmd)
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

void FreeLook::overrideView(CViewSetup* view)
{
	if (!config.get<bool>(vars.bFreeLook))
		return;

	if (config.get<Key>(vars.kFreeLook).isEnabled())
	{
		if (m_lastAngleLook.isZero())
			return;

		view->m_angles.x = m_lastAngleLook.x;
		view->m_angles.y = m_lastAngleLook.y;
	}
}