#pragma once

#include <imgui.h>

namespace ImGui
{
	namespace ExtraDrawlist
	{
		void AddRectFilledMultiColor(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max,
			ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left, float rounding, ImDrawFlags flags = 0);
		void AddConvexPolyFilledMultiColor(ImDrawList* draw_list, const ImVec2* points, const ImU32* col, const int points_count);
	}
}