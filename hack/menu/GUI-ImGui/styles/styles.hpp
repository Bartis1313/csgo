#pragma once

#include <imgui.h>
#include <array>
#include <config/cfgcolor.hpp>

struct ImStyleCustom
{
	ImGuiStyle style{ };
	std::array<CfgColor, 4> extraColors{ };
};

namespace styles
{
	ImStyleCustom getCherryStyle();
}