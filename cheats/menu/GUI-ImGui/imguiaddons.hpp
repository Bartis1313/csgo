#pragma once
#include <span>
#include <string>
#include <vector>
#include "../../../dependencies/ImGui/imgui.h"
// this file contains extra widgets

class Color;
using ImGuiColorEditFlags = int;

namespace ImGui
{
	// basic hotkey function, that behaves like button.
	void Hotkey(const char* label, int* key, const ImVec2& size = { 0.0f, 0.0f });
	// from demo
	void HelpMarker(const char* desc);
	bool ColorPicker(const char* label, Color* clr, const ImGuiColorEditFlags flags = 0);

	// from widgets, https://github.com/ocornut/imgui/issues/1496#issuecomment-569892444
	void BeginGroupPanel(const char* name, const ImVec2& size = { 0.0f, 0.0f });
	void EndGroupPanel();
	bool Combo(const char* label, int* item, const std::span<const char*>& arr, const float width = 90.0f);
	bool ListBox(const char* label, int* item, const std::vector<std::string>& arr, const int heightItem = -1);
	// what is now left are tabs, to currrently handle them by sizes or auto size by menu window width
	// it will prolly be impissible, so in near future I am going to rewite them and put versions here
	// THIS is fixed by poorly made "hacky" way in imgui by forcing sizes, and then backup the original. This is easy solution.
	// if you are reading this, and have own good tabs, please make a pull request
}
