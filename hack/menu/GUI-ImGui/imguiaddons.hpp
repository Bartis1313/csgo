#pragma once

#include <string>
#include <vector>

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