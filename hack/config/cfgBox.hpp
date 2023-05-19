#pragma once

#include "cfgcolor.hpp"

struct CfgBox
{
	int mode{ 0 };
	bool outline{ false };
	bool gradient{ false };
	CfgColor color{ };
	CfgColor fill{ };

	CfgColor gradientCol1{ };
	CfgColor gradientCol2{ };
	CfgColor gradientCol3{ };
	CfgColor gradientCol4{ };
};