#pragma once

#include <span>
#include <string>
#include <vector>
#include <format>
#include <functional>
#include <unordered_map>

#include <imgui.h>

class CfgColor;
class Color;
class Key;
using ImGuiColorEditFlags = int;

namespace ImGui
{
	// from demo
	void HelpMarker(const char* desc);
	// from widgets, https://github.com/ocornut/imgui/issues/1496#issuecomment-569892444
	void BeginGroupPanel(const char* name, const ImVec2& size = { 0.0f, 0.0f });
	void EndGroupPanel();
	// from demo with slight changes
	void ShowStyleEditorCfg(ImGuiStyle* ref);
}

IMGUI_IMPL_API void* ImGui_CreateTexture(const void* data, int width, int height);
IMGUI_IMPL_API void ImGui_DestroyTexture(void* texture);