#include "styles.hpp"

#include <dependencies/ImGui/imgui.h>

void styles::runCherry()
{
	// cherry colors, 3 intensities
#define HI(v)   ImVec4(0.502f, 0.075f, 0.256f, v)
#define MED(v)  ImVec4(0.455f, 0.198f, 0.301f, v)
#define LOW(v)  ImVec4(0.232f, 0.201f, 0.271f, v)
// backgrounds (@todo: complete with BG_MED, BG_LOW)
#define BG(v)   ImVec4(0.200f, 0.220f, 0.270f, v)
// text
#define TEXT_(v) ImVec4(0.860f, 0.930f, 0.890f, v)

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_Text] = TEXT_(0.78f);
	style.Colors[ImGuiCol_TextDisabled] = TEXT_(0.28f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
	style.Colors[ImGuiCol_ChildBg] = BG(0.58f);
	style.Colors[ImGuiCol_PopupBg] = BG(0.9f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = BG(1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = MED(0.78f);
	style.Colors[ImGuiCol_FrameBgActive] = MED(1.00f);
	style.Colors[ImGuiCol_TitleBg] = LOW(1.00f);
	style.Colors[ImGuiCol_TitleBgActive] = HI(1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = BG(0.75f);
	style.Colors[ImGuiCol_MenuBarBg] = BG(0.47f);
	style.Colors[ImGuiCol_ScrollbarBg] = BG(1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = MED(0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = MED(1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
	style.Colors[ImGuiCol_ButtonHovered] = MED(0.86f);
	style.Colors[ImGuiCol_ButtonActive] = MED(1.00f);
	style.Colors[ImGuiCol_Header] = MED(0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = MED(0.86f);
	style.Colors[ImGuiCol_HeaderActive] = HI(1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
	style.Colors[ImGuiCol_ResizeGripHovered] = MED(0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = MED(1.00f);
	style.Colors[ImGuiCol_PlotLines] = TEXT_(0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = MED(1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = TEXT_(0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = MED(1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = MED(0.43f);
	// [...]
	style.Colors[ImGuiCol_ModalWindowDimBg] = BG(0.73f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.539f, 0.479f, 0.255f, 0.162f);
	style.Colors[ImGuiCol_Tab] = MED(0.70f);
	style.Colors[ImGuiCol_TabHovered] = LOW(0.80f);
	style.Colors[ImGuiCol_TabActive] = HI(1.0f);

	style.ScrollbarSize = 8.0f;
	style.WindowRounding = 8.0f;
	style.Alpha = 1.0f;
	style.TabRounding = 5.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ChildRounding = 5.0f;
	style.ScrollbarRounding = 10.0f;

#undef HI
#undef MED
#undef LOW
#undef BG
#undef TEXT
}

void styles::runDark()
{
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();

	style.ScrollbarSize = 8.0f;
	style.WindowRounding = 8.0f;
	style.Alpha = 1.0f;
	style.TabRounding = 5.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ChildRounding = 5.0f;
	style.ScrollbarRounding = 10.0f;
}

void styles::runWhite()
{
	ImGui::StyleColorsLight();

	ImGuiStyle& style = ImGui::GetStyle();

	style.ScrollbarSize = 8.0f;
	style.WindowRounding = 8.0f;
	style.Alpha = 1.0f;
	style.TabRounding = 5.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ChildRounding = 5.0f;
	style.ScrollbarRounding = 10.0f;
}

void styles::runClassic()
{
	ImGui::StyleColorsClassic();

	ImGuiStyle& style = ImGui::GetStyle();

	style.ScrollbarSize = 8.0f;
	style.WindowRounding = 8.0f;
	style.Alpha = 1.0f;
	style.TabRounding = 5.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ChildRounding = 5.0f;
	style.ScrollbarRounding = 10.0f;
}