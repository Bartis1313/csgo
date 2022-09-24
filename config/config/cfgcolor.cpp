#include "cfgcolor.hpp"

#include <cmath>
#include <numbers>

#include <SDK/CGlobalVars.hpp>
#include <SDK/interfaces/interfaces.hpp>

CfgColor::CfgColor(const Color& color, bool rainbow, float speed)
	: m_color{ color }, m_rainbow{ rainbow }, m_speed{ speed }
{}

CfgColor::CfgColor(const CfgColor& other)
{
	*this = other;
	refresh();
}

CfgColor& CfgColor::operator=(const CfgColor& other)
{
	this->m_color = other.getColor();
	this->m_rainbow = other.getRainbow();
	this->m_speed = other.getSpeed();
	//this->refresh();
	return *this;
}
CfgColor& CfgColor::operator=(const Color& other)
{
	this->m_color = other;
	return *this;
}

void CfgColor::updateCol(float gameTime, float multiply)
{
	this->m_color.at(0) = std::cos(gameTime * multiply) * 0.5f + 0.5f;
	this->m_color.at(1) = std::cos(gameTime * multiply - 2.0f * std::numbers::pi_v<float> / 3.0f) * 0.5f + 0.5f;
	this->m_color.at(2) = std::cos(gameTime * multiply - 4.0f * std::numbers::pi_v<float> / 3.0f) * 0.5f + 0.5f;
}

void CfgColor::refresh()
{
	if (interfaces::globalVars)
	{
		if (this->getRainbow())
			this->updateCol(interfaces::globalVars->m_realtime, this->getSpeed());
	}
}