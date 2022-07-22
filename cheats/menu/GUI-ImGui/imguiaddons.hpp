#pragma once

#include <span>
#include <string>
#include <vector>
#include <locale>
#include <format>

#include "../../../dependencies/ImGui/imgui.h"

class CfgColor;
class Key;
using ImGuiColorEditFlags = int;

namespace ImGui
{
	// basic hotkey function, that behaves like button.
	void Hotkey(const char* label, Key* key, bool useExtended = true, const ImVec2& size = { 0.0f, 0.0f });
	// from demo
	void HelpMarker(const char* desc);
	// heavily based on code from demo, look "palette" in imgui demo file
	bool ColorPicker(const char* label, CfgColor* clr);

	// from widgets, https://github.com/ocornut/imgui/issues/1496#issuecomment-569892444
	void BeginGroupPanel(const char* name, const ImVec2& size = { 0.0f, 0.0f });
	void EndGroupPanel();
	bool Combo(const char* label, int* item, std::span<const char*> arr, const float width = -1.0f);
	bool Combo(const char* label, int* item, std::span<const std::string> arr, const float width = -1.0f);
	bool ListBox(const char* label, int* item, std::span<const char*> arr, const int heightItem = -1);
	bool ListBox(const char* label, int* item, std::span<const std::string> arr, const int heightItem = -1);
	// need return anything? maybe, but useless at all for my use
	void MultiCombo(const char* label, const std::span<const char*>& names, std::vector<bool>& options, const float width = 140.0f);

	// from demo, slight edit, usage same as normal console.log
	struct ExampleAppLog
	{
		ImGuiTextBuffer     Buf;
		ImGuiTextFilter     Filter;
		ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
		bool                AutoScroll;  // Keep scrolling if already at the bottom.

		ExampleAppLog();
		void Clear();

		// edited 
		template<typename... Args_t>
		void AddLog(const std::string& fmt, const Args_t&... args);
		void Draw(const char* title, bool* p_open = NULL);
	};
}

template<typename... Args_t>
void ImGui::ExampleAppLog::AddLog(const std::string& fmt, const Args_t&... args)
{
	if (fmt.empty())
		return;

	std::string res;

	if constexpr (sizeof...(args) > 0)
		res += std::vformat(std::locale(), fmt, std::make_format_args(args...));
	else
		res += fmt;

	int oldSize = Buf.size();

	Buf.append(res.c_str());

	for (int newSize = Buf.size(); oldSize < newSize; oldSize++)
		if (Buf[oldSize] == '\n')
			LineOffsets.push_back(oldSize + 1);
}