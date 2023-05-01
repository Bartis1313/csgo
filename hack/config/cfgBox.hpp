#pragma once

#include "cfgcolor.hpp"

struct CfgBox
{
	bool enabled{ false };
	bool fill{ false };
	bool gradient{ false };
	bool outline{ false };
	float thickness{ 2.0f };

	CfgColor color{ };
	CfgColor fillColor{ };
	CfgColor outlineColor{ };

	CfgColor gradientColor1{ };
	CfgColor gradientColor2{ };
	CfgColor gradientColor3{ };
	CfgColor gradientColor4{ };
};