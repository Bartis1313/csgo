#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

#include <imgui.h>

class CfgColor;
class Color;
class Key;

struct ImNotify
{
    ImNotify() = default;
    ImNotify(double maxTime, const std::string& title, const std::string& message)
        : maxTime{ maxTime }, title{ title }, message{ message }
    {
        time = ImGui::GetTime();
    }

    double maxTime;
    std::string title;
    std::string message;
    double time;
    float alpha;

    static void handle();
    static void add(const ImNotify& notify);
    inline static std::vector<ImNotify> m_allNotifies;
};

// ImGuiWindowSettings wrapper
struct ImWindowSettings
{
    ImGuiID id;
    ImVec2 pos, size;
    ImVec2 targetSize;
    float alpha;
};

namespace ImGui
{
	// from demo
	void HelpMarker(const char* desc);
	// from widgets, https://github.com/ocornut/imgui/issues/1496#issuecomment-569892444
	void BeginGroupPanel(const char* name, const ImVec2& size = { 0.0f, 0.0f });
	void EndGroupPanel();

	template<class T>
	concept dataLike = requires(T t)
	{
		t.size();
		t.data();
		t.begin();
		t.end();
	};

	// heavily based on code from demo, look "palette" in imgui demo file
	bool ColorPicker(const char* label, CfgColor* clr);
	bool PopupButton(const char* label, const std::function<void()>& fun);
	bool Hotkey(const char* label, Key* key, bool useExtended = true, const ImVec2& size = { 0.0f, 0.0f });
	template<dataLike T>
	bool Combo(const char* label, int* item, const T& data, int height_in_items = -1);
	template<dataLike T>
	bool ListBox(const char* label, int* item, const T& data, int height_in_items = -1);
	template<typename T, size_t SIZE>
	requires (SIZE > 0U)
	void MultiCombo(const char* label, const std::array<T, SIZE>& names, std::array<bool, SIZE>* options);
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

    void LoadCustomSettings();
    namespace extraGlobals
    {
        inline std::unordered_map<ImGuiID, ImWindowSettings> settings;
    }
}

template<typename T, size_t SIZE>
requires (SIZE > 0U)
void ImGui::MultiCombo(const char* label, const std::array<T, SIZE>& names, std::array<bool, SIZE>* options)
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

	if (ImGui::BeginCombo(label, previewName.c_str()))
	{
		for (size_t i = 0; i < SIZE; i++) // creating view to make it "forced" to detect it as valid function args
		{
			if (ImGui::Selectable(std::string_view{ names[i] }.data(), options->at(i), ImGuiSelectableFlags_DontClosePopups))
				options->at(i) = !options->at(i);
		}

		ImGui::EndCombo();
	}
}

template<ImGui::dataLike T>
bool ImGui::Combo(const char* label, int* item, const T& data, int height_in_items)
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

	return ImGui::Combo(label, item, getter_arr, const_cast<void*>(reinterpret_cast<const void*>(&data)), data.size(), height_in_items);
}

template<ImGui::dataLike T>
bool ImGui::ListBox(const char* label, int* item, const T& data, int height_in_items)
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

	return ImGui::ListBox(label, item, getter_arr, const_cast<void*>(reinterpret_cast<const void*>(&data)), data.size(), height_in_items);
}



IMGUI_IMPL_API void* ImGui_CreateTexture(const void* data, int width, int height);
IMGUI_IMPL_API void ImGui_DestroyTexture(void* texture);