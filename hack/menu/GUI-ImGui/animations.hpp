#pragma once

#include <imgui.h>
#include <imgui_internal.h>

#include <unordered_map>
#include <functional>
#include <span>

class CfgColor;
class Key;

namespace ImGui
{
	namespace Animations
	{
		namespace detail
		{
			inline float speedAlpha = 0.0f;
			inline float speedOutline = 0.0f;
			inline float speedPop = 0.0f;
			inline float speedMove = 10.0f;
		}
		void setAnimationSpeeds();

		struct AlphaAnimation
		{
			float alpha{};
			float outline{};
		};

		struct MoveAnimation
		{
			float move{};
		};

		struct AlphaLimits
		{
			float alphaLimit{};
			float outlineLimit{};
		};

		struct SingleAnimation
		{
			float value{};
		};

		struct SingleField
		{
			float value{};
		};

		inline std::unordered_map<ImGuiID, AlphaAnimation> alphas;
		inline std::unordered_map<ImGuiID, MoveAnimation> moves;
		inline std::unordered_map<ImGuiID, SingleAnimation> singleAnim;
		inline std::unordered_map<ImGuiID, SingleField> singleFields;
		inline std::unordered_map<ImGuiID, float> timeStack; // fixed widget time based on their id. Not using it globally

		// functions seperated, don't return Animation as object. Instead contruct it on own in widget
		[[nodiscard]] AlphaAnimation getAlpha(ImGuiID id, const AlphaLimits& limits);
		[[nodiscard]] MoveAnimation getMove(ImGuiID id, float lastTime, bool held, bool pressed);
		[[nodiscard]] SingleAnimation getSingleAnimation(ImGuiID id, float max, float duration, float extraSpped = 1.0f);
		[[nodiscard]] SingleField getAndUpdateSingleField(ImGuiID id, float value);
		[[nodiscard]] float getLastTime(ImGuiID id, bool held, bool pressed);
		template<typename T>
		[[nodiscard]] static inline T ImLerpSmooth(T a, T b, float t, float smoothness);

		template<class T>
		concept dataLike = requires(T t)
		{
			t.size();
			t.data();
			t.begin();
			t.end();
		};

		bool Hotkey(const char* label, Key* key, bool useExtended = true, const ImVec2& size = { 0.0f, 0.0f });
		bool Checkbox(const char* label, bool* v);
		bool BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags = 0);
		bool Combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items);
		bool Combo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items);
		template<dataLike T>
		bool Combo(const char* label, int* item, const T& data, int height_in_items = -1);
		template<dataLike T>
		bool ListBox(const char* label, int* item, const T& data, int height_in_items = -1);
		template<typename T, size_t SIZE>
		requires (SIZE > 0U)
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
		bool ColorButton(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags = 0, ImVec2 size = ImVec2(0, 0));
		bool ListBox(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int height_in_items);
		// heavily based on code from demo, look "palette" in imgui demo file
		bool ColorPicker(const char* label, CfgColor* clr);
		bool PopupButton(const char* label, const std::function<void()>& fun);

		bool Selectable(const char* label, bool selected, ImGuiSelectableFlags flags = 0, const ImVec2& size_arg = ImVec2(0, 0));
		void RenderTextAlpha(ImVec2 pos, const char* text, const char* text_end, bool hide_text_after_hash, float alpha);
		void RenderCheckMarkExtra(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float sz, float thickness_extra);
		void RenderFrameAlpha(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding, float alpha);
		void RenderFrameAlphaOutline(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding, float alpha, float outline);
		void RenderFrameOutlineKeepFill(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding, float outline);
		void RenderFrameBorderAlpha(ImVec2 p_min, ImVec2 p_max, float rounding, float alpha);
		void RenderArrow(ImDrawList* draw_list, ImVec2 pos, ImU32 col, ImGuiDir dir, float angle, float scale);
		bool BeginComboPopup(ImGuiID popup_id, const ImRect& bb, ImGuiComboFlags flags, float pop);
		
		struct WindowConfig
		{
			ImGuiWindowFlags flags{ 0 };

			// will only work at the very first start, later it will be saved in config
			ImVec2 defaultPos, defaultSize;
		};

		// DONT USE NOW, this is working, but saving is completely broken with this
		// reasoning is:
		// I find using ImGuiStorage for this painfull
		// This manipulates how imgui originally should handle stack window openinig/closing.
		void Window(const char* label, bool* opened, const WindowConfig& config, const std::function<void()>& callback);
	}
}

template<typename T, size_t SIZE>
	requires (SIZE > 0U)
void ImGui::Animations::MultiCombo(const char* label, const std::array<T, SIZE>& names, std::array<bool, SIZE>* options)
{
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

	if (ImGui::Animations::BeginCombo(label, previewName.c_str()))
	{
		for (size_t i = 0; i < SIZE; i++) // creating view to make it "forced" to detect it as valid function args
		{
			if (ImGui::Animations::Selectable(std::string_view{ names[i] }.data(), options->at(i), ImGuiSelectableFlags_DontClosePopups))
				options->at(i) = !options->at(i);
		}

		ImGui::EndCombo();
	}
}

template<ImGui::Animations::dataLike T>
bool ImGui::Animations::Combo(const char* label, int* item, const T& data, int height_in_items)
{
	using typeContainer = typename std::decay<decltype(*data.begin())>::type;

	auto getter_arr = [](void* data, int idx, const char** out)
	{
		auto& v = *static_cast<T*>(data);

		if (out)
		{
			if constexpr (std::is_same_v<typeContainer, std::string>)
				*out = v[idx].c_str();
			else if constexpr (std::is_same_v<typeContainer, std::string_view>)
				*out = v[idx].data();
			else
				*out = v[idx];
		}
		return true;
	};

	return ImGui::Animations::Combo(label, item, getter_arr, const_cast<void*>(reinterpret_cast<const void*>(&data)), data.size(), height_in_items);
}

template<ImGui::Animations::dataLike T>
bool ImGui::Animations::ListBox(const char* label, int* item, const T& data, int height_in_items)
{
	using typeContainer = typename std::decay<decltype(*data.begin())>::type;

	auto getter_arr = [](void* data, int idx, const char** out)
	{
		auto& v = *static_cast<T*>(data);

		if (out)
		{
			if constexpr (std::is_same_v<typeContainer, std::string>)
				*out = v[idx].c_str();
			else if constexpr (std::is_same_v<typeContainer, std::string_view>)
				*out = v[idx].data();
			else
				*out = v[idx];
		}
		return true;
	};

	return ImGui::Animations::ListBox(label, item, getter_arr, const_cast<void*>(reinterpret_cast<const void*>(&data)), data.size(), height_in_items);
}

template<typename T>
static inline T ImGui::Animations::ImLerpSmooth(T a, T b, float t, float smoothness)
{
	t = (t > 1.0f) ? 1.0f : ((t < 0.0f) ? 0.0f : t);
	float interp = t * t * (3.0f - 2.0f * t);
	return (T)(a + (b - a) * (interp * smoothness + (1.0f - smoothness) * t));
}