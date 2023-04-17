#pragma once

#include "cfgcolor.hpp"

struct CfgBeam
{
	int index{ 0 };
	std::string flags{ "4" };
	float life{ 2.0f };
	CfgColor color{ Colors::LightBlue };
	float width{ 2.0f };
	float fadeLength{ 1.0f };
	float amplitude{ 2.0f };
	float speed{ 0.0f };
	float startFrame{ 0.0f };
	float frameRate{ 0.0f };
	int segments{ 2 };
};