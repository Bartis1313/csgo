#pragma once

#include "cfgcolor.hpp"

struct CfgCham
{
	bool enabled{ false };
	bool hide{ };
	bool ignorez{ };
	bool wireframe{ };
	int index{ 0 };
	CfgColor color{ };
	CfgColor colorXQZ{ };
};