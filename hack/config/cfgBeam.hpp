#pragma once

#include "cfgcolor.hpp"

struct CfgBeam
{
	int index;
	std::string flags;
	float life;
	CfgColor color;
	float width;
	float fadeLength;
	float amplitude;
	float speed;
	float startFrame;
	float frameRate;
	int segments;
};