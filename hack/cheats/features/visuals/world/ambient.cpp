#include "ambient.hpp"

#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/Enums.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <utilities/tools/tools.hpp>
#include <config/vars.hpp>
#include <cheats/game/globals.hpp>

// C7 44 24 ? ? ? ? ? 8B 84 24 ? ? ? ? in sdsshader_dx9 looks to be an array of those
void AmbientLight::init()
{
	m_ambientR = memory::interfaces::cvar->findVar("mat_ambient_light_r");
	m_ambientG = memory::interfaces::cvar->findVar("mat_ambient_light_g");
	m_ambientB = memory::interfaces::cvar->findVar("mat_ambient_light_b");
}

void AmbientLight::run(int frame)
{
	if (frame != FRAME_RENDER_START)
		return;

	if (auto cfg = vars::visuals->world->ambient->enabled; cfg && (m_buttonState || m_pickerState))
	{
		auto cfgCol = vars::visuals->world->ambient->color();

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