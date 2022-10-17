#pragma once

#include <span>
#include <string>
#include <vector>
#include <format>
#include <functional>

#include <dependencies/ImGui/imgui.h>

class CfgColor;
class Color;
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
	bool Combo(const char* label, int* item, std::span<const char*> arr, const int = -1);
	bool Combo(const char* label, int* item, std::span<const std::string> arr, const int width = -1);
	bool ListBox(const char* label, int* item, std::span<const char*> arr, const int heightItem = -1);
	bool ListBox(const char* label, int* item, std::span<const std::string> arr, const int heightItem = -1);	
	template<typename T, size_t SIZE>
	void MultiCombo(const char* label, const std::array<T, SIZE>& names, std::array<bool, SIZE>* options);
	bool PopupButton(const char* label, const std::function<void()>& fun);

	// from demo, slight edit, usage same as normal console.log
	struct ExampleAppLog
	{
		ImGuiTextBuffer     Buf;
		ImGuiTextFilter     Filter;
		ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
		bool                AutoScroll;  // Keep scrolling if already at the bottom.

		ExampleAppLog();
		void Clear();

		void AddLog(const std::string& buffer);
		void Draw(const char* title, bool* p_open = NULL);
	};
}

IMGUI_IMPL_API void* ImGui_CreateTexture(const void* data, int width, int height);
IMGUI_IMPL_API void	ImGui_DestroyTexture(void* texture);

template<typename T, size_t SIZE>
void ImGui::MultiCombo(const char* label, const std::array<T, SIZE>& names, std::array<bool, SIZE>* options)
{
	bool check = names.size() != options->size() || !names.empty() || !options->empty();
	assert(check && "given size of arrays args was not equal or one of them was empty");

	size_t size = names.size(); // does not matter if you pass options size here

	ImVector<T> actives = {};
	for (size_t i = 0; const auto el : *options)
	{
		if (el) // if active selected
			actives.push_back(names[i]);

		i++;
	}

	std::string previewName = "";
	for (int i = 0; const auto & el : actives)
	{
		previewName += el;

		if (i < actives.size() - 1) // add ", " on every option but not last
			previewName += ", ";

		i++;
	}

	if (ImGui::BeginCombo(label, previewName.c_str()))
	{
		for (size_t i = 0; i < size; i++) // creating view to make it "forced" to detect it as valid function args
		{
			if (ImGui::Selectable(std::string_view{ names[i] }.data(), options->at(i), ImGuiSelectableFlags_DontClosePopups))
				options->at(i) = !options->at(i);
		}

		ImGui::EndCombo();
	}
}