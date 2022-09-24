#include "removeSky.hpp"

#include <SDK/ICvar.hpp>
#include <SDK/Convar.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>

void RemoveSky::init()
{
	m_sky = interfaces::cvar->findVar(XOR("r_3dsky"));
}

void RemoveSky::run()
{
	config.get<bool>(vars.bRemoveSky) ? m_sky->setValue(false) : m_sky->setValue(true);
}
