#include "cfgcolor.hpp"

#include <cmath>
#include <numbers>

#include <SDK/CGlobalVars.hpp>
#include <SDK/interfaces/interfaces.hpp>

#include "cfgcolor.hpp"

#include <cmath>
#include <numbers>

#include <SDK/CGlobalVars.hpp>
#include <gamememory/memory.hpp>

void CfgColor::updateCol(float gameTime, float multiply)
{
	this->m_color.at(0) = std::cos(gameTime * multiply) * 0.5f + 0.5f;
	this->m_color.at(1) = std::cos(gameTime * multiply - 2.0f * std::numbers::pi_v<float> / 3.0f) * 0.5f + 0.5f;
	this->m_color.at(2) = std::cos(gameTime * multiply - 4.0f * std::numbers::pi_v<float> / 3.0f) * 0.5f + 0.5f;
}

void CfgColor::refresh()
{
	if (memory::interfaces::globalVars())
	{
		if (this->getRainbow())
			this->updateCol(memory::interfaces::globalVars->m_realtime, this->getSpeed());
	}
}