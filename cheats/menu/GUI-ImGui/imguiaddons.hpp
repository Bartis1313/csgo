#pragma once

#include <span>
#include <string>
#include <vector>
#include <format>
#include <functional>
#include <unordered_map>

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

	// from demo with slight changes
	void ShowStyleEditorCfg(ImGuiStyle* ref);

	namespace Animated // https://github.com/ocornut/imgui/issues/1857 WIP
	{
		
		struct CheckboxAnim
		{
			float alpha = 0.0f; // handle by clamping min/max with deltatime
			float thickness = 0.0f; // handle by saturate
			float checkMark = 0.0f;
			float lastTime = 0.0f; // handle it on own!
			float alpha_outline = 0.0f;

			constexpr CheckboxAnim()
				: alpha{ 0.0f }, thickness{ 0.0f }, checkMark{ 0.0f }
			{}
			constexpr CheckboxAnim(float a, float t, float cm, float time, float o)
				: alpha{ a }, thickness{ t }, checkMark{ cm }, lastTime{ time }, alpha_outline{ o }
			{}
		};

		struct ButtonAnim
		{
			float alpha = 0.0f; // handle by clamping min/max with deltatime
			float thickness = 0.0f; // handle by saturate
			float lastTime = 0.0f; // handle it on own!
			float alpha_outline = 0.0f;

			constexpr ButtonAnim()
				: alpha{ 0.0f }, thickness{ 0.0f }, lastTime{ 0.0f }, alpha_outline{ 0.0f }
			{}
			constexpr ButtonAnim(float a, float t, float time, float o)
				: alpha{ a }, thickness{ t }, lastTime{ time }, alpha_outline{ o }
			{}
		};

		struct SingleAnim
		{
			float alpha = 0.0f; // handle by clamping min/max with deltatime

			constexpr SingleAnim()
				: alpha{ 0.0f }
			{}
			constexpr SingleAnim(float a)
				: alpha{ a }
			{}
		};

		struct AlphaAnimOutline
		{
			float alpha = 0.0f; // handle by clamping min/max with deltatime
			float alpha_outline = 0.0f;

			constexpr AlphaAnimOutline()
				: alpha{ 0.0f }
			{}
			constexpr AlphaAnimOutline(float a, float o)
				: alpha{ a }, alpha_outline{ o }
			{}
		};

		bool Checkbox(const char* label, bool* v);
		bool BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags = 0);
		bool Combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items);
		bool Combo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items);
		template<typename T>
		bool Combo(const char* label, int* item, std::span<T> arr, int height_in_items = -1);
		template<typename T>
		bool ListBox(const char* label, int* item, std::span<T> arr, int height_in_items = -1);
		template<typename T, size_t SIZE>
		void MultiCombo(const char* label, const std::array<T, SIZE>& names, std::array<bool, SIZE>* options);
		bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);     // adjust format to decorate the value with a prefix or a suffix for in-slider labels or unit display.
		bool SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
		bool SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
		bool SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
		bool SliderAngle(const char* label, float* v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f, const char* format = "%.0f deg", ImGuiSliderFlags flags = 0);
		bool SliderInt(const char* label, int* v, int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
		bool SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
		bool SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
		bool SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* format = "%d", ImGuiSliderFlags flags = 0);
		bool SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format = NULL, ImGuiSliderFlags flags = 0);
		bool ButtonEx(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags);
		bool Button(const char* label, const ImVec2& size_arg = ImVec2(0, 0));
		//bool ColorButton(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags = 0, ImVec2 size = ImVec2(0, 0));
		bool ListBox(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int height_in_items);

		bool Selectable(const char* label, bool selected, ImGuiSelectableFlags flags = 0, const ImVec2& size_arg = ImVec2(0, 0));
		void RenderTextAlpha(ImVec2 pos, const char* text, const char* text_end, bool hide_text_after_hash, float alpha);
		void RenderCheckMarkExtra(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float sz, float thickness_extra);
		void RenderFrameAlpha(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding, float alpha);
		void RenderFrameAlphaOutline(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding, float alpha, float outline);
		void RenderFrameOutlineKeepFill(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding, float outline);
		void RenderFrameBorderAlpha(ImVec2 p_min, ImVec2 p_max, float rounding, float alpha);	
	}
}

IMGUI_IMPL_API void* ImGui_CreateTexture(const void* data, int width, int height);
IMGUI_IMPL_API void	ImGui_DestroyTexture(void* texture);

template<typename T, size_t SIZE>
void ImGui::Animated::MultiCombo(const char* label, const std::array<T, SIZE>& names, std::array<bool, SIZE>* options)
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

	if (ImGui::Animated::BeginCombo(label, previewName.c_str()))
	{
		for (size_t i = 0; i < size; i++) // creating view to make it "forced" to detect it as valid function args
		{
			if (ImGui::Animated::Selectable(std::string_view{ names[i] }.data(), options->at(i), ImGuiSelectableFlags_DontClosePopups))
				options->at(i) = !options->at(i);
		}

		ImGui::EndCombo();
	}
}

template<typename T>
bool ImGui::Animated::Combo(const char* label, int* item, std::span<T> arr, int height_in_items)
{
	auto getter_arr = [](void* data, int idx, const char** out)
	{
		auto& v = *static_cast<std::span<T>*>(data);

		if (out)
		{
			if constexpr (std::is_same_v<T, const std::string>)
				*out = v[idx].c_str();
			else
				*out = v[idx];
		}
		return true;
	};

	return ImGui::Animated::Combo(label, item, getter_arr, const_cast<void*>(reinterpret_cast<const void*>(&arr)), arr.size(), height_in_items);
}

template<typename T>
bool ImGui::Animated::ListBox(const char* label, int* item, std::span<T> arr, int height_in_items)
{
	auto getter_arr = [](void* data, int idx, const char** out)
	{
		auto& v = *static_cast<std::span<T>*>(data);

		if (out)
		{
			if constexpr (std::is_same_v<T, const std::string>)
				*out = v[idx].c_str();
			else
				*out = v[idx];
		}
		return true;
	};

	return ImGui::Animated::ListBox(label, item, getter_arr, const_cast<void*>(reinterpret_cast<const void*>(&arr)), arr.size(), height_in_items);
}