#include "ambient.hpp"

#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/Enums.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <utilities/tools/tools.hpp>
#include <config/vars.hpp>
#include <game/globals.hpp>

void AmbientLight::init()
{
	m_ambientR = interfaces::cvar->findVar(XOR("mat_ambient_light_r"));
	m_ambientG = interfaces::cvar->findVar(XOR("mat_ambient_light_g"));
	m_ambientB = interfaces::cvar->findVar(XOR("mat_ambient_light_b"));
}

void AmbientLight::run(int frame)
{
	if (frame != FRAME_RENDER_START)
		return;

	if (auto cfg = config.get<bool>(vars.bAmbientLight); cfg && (m_buttonState || m_pickerState))
	{
		auto cfgCol = config.get<CfgColor>(vars.cAmbientLight).getColor();

		m_ambientR->setValue(cfgCol.r());
		m_ambientG->setValue(cfgCol.g());
		m_ambientB->setValue(cfgCol.b());

	}
	else if (globals::isShutdown || (!cfg && m_buttonState))
	{
		m_ambientR->setValue(0.0f);
		m_ambientG->setValue(0.0f);
		m_ambientB->setValue(0.0f);
	}
}