#include "imguiaddons.hpp"

#include "menu.hpp"

#include <config/config.hpp>
#include <utilities/utilities.hpp>
#include <cheats/game/globals.hpp>

#include <imgui_internal.h>

void ImNotify::handle()
{
	const ImVec2 screenSize = ImGui::GetIO().DisplaySize;
	constexpr float widthRect = 200.0f;
	constexpr float fineClip = 5.0f;

	constexpr float animationOUT = 0.5f;
	constexpr float animationIN = 0.02f; // bigger values wont look good cuz its small pos difference

	float height = fineClip;
	for (size_t i = 0; auto& notify : m_allNotifies)
	{
		const float in = static_cast<float>(ImClamp((ImGui::GetTime() - notify.time) / animationIN, 0.95, 1.0));
		const float out = static_cast<float>(ImClamp(((ImGui::GetTime() - notify.time) - notify.maxTime) / animationOUT, 0.0, 1.0));

		const float alpha = in * (1.0f - out);
		notify.alpha = alpha;

		if (notify.alpha <= 0.0f)
		{
			m_allNotifies.erase(m_allNotifies.begin() + i);
			continue;
		}

		char name[15];
		ImFormatString(name, IM_ARRAYSIZE(name), "##UNIQUE%i", i);
		ImGui::SetNextWindowBgAlpha(alpha);

		const float initX = screenSize.x - fineClip;
		const float xAnimated = in * initX + out * initX;
		ImGui::SetNextWindowPos(ImVec2{ xAnimated, height + fineClip }, ImGuiCond_Always, ImVec2{ 1.0f, 0.0f });
		ImGui::Begin(name, nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs
			| ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoFocusOnAppearing);
		{
			ImGui::PushTextWrapPos(widthRect);

			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_Text, alpha));
			ImGui::TextUnformatted(notify.title.c_str());
			ImGui::Separator();
			ImGui::TextUnformatted(notify.message.c_str());

			ImGui::PopStyleColor();

			ImGui::PopTextWrapPos();
		}

		height += (ImGui::GetWindowHeight() + fineClip) * alpha;

		ImGui::End();

		i++;
	}
}

void ImNotify::add(const ImNotify& notify)
{
	m_allNotifies.push_back(notify);
}

void ImGui::HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

#include <config/cfgcolor.hpp>

#include <config/vars.hpp>
#include <unordered_map>
#include <render/render.hpp>
#include <deps/ImGui/extraDraw.hpp>

static std::unordered_map<ImGuiID, std::pair<ImVec2, ImVec2>> m_mapSizes = {};
static const char* g_GroupPanelName = nullptr;
static ImVector<ImRect> s_GroupPanelLabelStack;

void ImGui::BeginGroupPanel(const char* name, const ImVec2& size)
{
	g_GroupPanelName = name;

	auto id = ImGui::GetCurrentWindow()->GetID(name);
	auto [min, max] = m_mapSizes[id];

	ImGui::ExtraDrawlist::AddRectFilledMultiColor(
		ImGui::GetWindowDrawList(),
		min, max,
		Color::U32(vars::styling->groupPanelBackground[0]()),
		Color::U32(vars::styling->groupPanelBackground[1]()),
		Color::U32(vars::styling->groupPanelBackground[2]()),
		Color::U32(vars::styling->groupPanelBackground[3]()),
		ImGui::GetStyle().FrameRounding
	);

	ImGui::BeginGroup();

	auto cursorPos = ImGui::GetCursorScreenPos();
	auto itemSpacing = ImGui::GetStyle().ItemSpacing;
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	auto frameHeight = ImGui::GetFrameHeight();
	ImGui::BeginGroup();

	ImVec2 effectiveSize = size;
	if (size.x < 0.0f)
		effectiveSize.x = ImGui::GetContentRegionAvail().x;
	else
		effectiveSize.x = size.x;
	ImGui::Dummy(ImVec2(effectiveSize.x, 0.0f));

	ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
	ImGui::SameLine(0.0f, 0.0f);
	ImGui::BeginGroup();
	ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
	ImGui::SameLine(0.0f, 0.0f);
	ImGui::PushFont(ImFonts::tahoma20);
	ImGui::TextUnformatted(name);
	ImGui::PopFont();
	auto labelMin = ImGui::GetItemRectMin();
	auto labelMax = ImGui::GetItemRectMax();
	ImGui::SameLine(0.0f, 0.0f);
	ImGui::Dummy(ImVec2(0.0, frameHeight + itemSpacing.y));
	ImGui::BeginGroup();

	//ImGui::GetWindowDrawList()->AddRect(labelMin, labelMax, IM_COL32(255, 0, 255, 255));

	ImGui::PopStyleVar(2);

	ImGui::GetCurrentWindow()->ContentRegionRect.Max.x -= frameHeight * 0.5f;
	ImGui::GetCurrentWindow()->WorkRect.Max.x -= frameHeight * 0.5f;
	ImGui::GetCurrentWindow()->InnerRect.Max.x -= frameHeight * 0.5f;

	ImGui::GetCurrentWindow()->Size.x -= frameHeight;

	auto itemWidth = ImGui::CalcItemWidth();
	ImGui::PushItemWidth(ImMax(0.0f, itemWidth - frameHeight));

	s_GroupPanelLabelStack.push_back(ImRect(labelMin, labelMax));
}

void ImGui::EndGroupPanel()
{
	ImGui::PopItemWidth();

	auto itemSpacing = ImGui::GetStyle().ItemSpacing;

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	auto frameHeight = ImGui::GetFrameHeight();

	ImGui::EndGroup();

	//ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0, 255, 0, 64), 4.0f);

	ImGui::EndGroup();

	ImGui::SameLine(0.0f, 0.0f);
	ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
	ImGui::Dummy(ImVec2(0.0, frameHeight - frameHeight * 0.5f - itemSpacing.y));

	ImGui::EndGroup();

	auto itemMin = ImGui::GetItemRectMin();
	auto itemMax = ImGui::GetItemRectMax();
	//ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 0, 0, 64), 4.0f);

	auto labelRect = s_GroupPanelLabelStack.back();
	s_GroupPanelLabelStack.pop_back();

	ImVec2 halfFrame = ImVec2(frameHeight * 0.25f, frameHeight) * 0.5f;
	ImRect frameRect = ImRect(itemMin + halfFrame, itemMax + ImVec2{ 0.0f, 2.0f });
	labelRect.Min.x -= itemSpacing.x;
	labelRect.Max.x += itemSpacing.x;
	const auto colorLine = Color::U32(vars::styling->groupPanelLines());
	for (int i = 0; i < 4; ++i)
	{
		switch (i)
		{
			// left half-plane
		case 0: ImGui::PushClipRect(ImVec2(-FLT_MAX, -FLT_MAX), ImVec2(labelRect.Min.x, FLT_MAX), true); break;
			// right half-plane
		case 1: ImGui::PushClipRect(ImVec2(labelRect.Max.x, -FLT_MAX), ImVec2(FLT_MAX, FLT_MAX), true); break;
			// top
		case 2: ImGui::PushClipRect(ImVec2(labelRect.Min.x, -FLT_MAX), ImVec2(labelRect.Max.x, labelRect.Min.y), true); break;
			// bottom
		case 3: ImGui::PushClipRect(ImVec2(labelRect.Min.x, labelRect.Max.y), ImVec2(labelRect.Max.x, FLT_MAX), true); break;
		}

		ImGui::GetWindowDrawList()->AddRect(
			frameRect.Min, frameRect.Max,
			colorLine,
			ImGui::GetStyle().FrameRounding);

		ImGui::PopClipRect();
	}

	auto id = ImGui::GetCurrentWindow()->GetID(g_GroupPanelName);

	auto [itr, emplaced] = m_mapSizes.try_emplace(id, std::make_pair(frameRect.Min, frameRect.Max));
	if (!emplaced)
		itr->second = std::make_pair(frameRect.Min, frameRect.Max);

	ImGui::PopStyleVar(2);

	ImGui::GetCurrentWindow()->ContentRegionRect.Max.x += frameHeight * 0.5f;
	ImGui::GetCurrentWindow()->WorkRect.Max.x += frameHeight * 0.5f;
	ImGui::GetCurrentWindow()->InnerRect.Max.x += frameHeight * 0.5f;

	ImGui::GetCurrentWindow()->Size.x += frameHeight;

	ImGui::Dummy(ImVec2(0.0f, 0.0f));

	ImGui::EndGroup();
}

#include <config/vars.hpp>

void ImGui::ShowStyleEditorCfg(ImGuiStyle* ref)
{
	// You can pass in a reference ImGuiStyle structure to compare to, revert to and save to
	// (without a reference style pointer, we will use one compared locally as a reference)
	ImGuiStyle& style = ImGui::GetStyle();
	static ImGuiStyle ref_saved_style;
	// ugly methods for custom colors, TODO: do own struct for this
	static auto ref_saved_gradient = vars::styling->groupPanelBackground;
	static auto ref_saved_color_panel_line = vars::styling->groupPanelLines;

	// Default to using internal storage as reference
	static bool init = true;
	if (init && ref == NULL)
		ref_saved_style = style;
	init = false;
	if (ref == NULL)
		ref = &ref_saved_style;

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

	if (ImGui::ShowStyleSelector("Colors##Selector"))
		ref_saved_style = style;
	ImGui::ShowFontSelector("Fonts##Selector");

	// Simplified Settings (expose floating-pointer border sizes as boolean representing 0.0f or 1.0f)
	if (ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f"))
		style.GrabRounding = style.FrameRounding; // Make GrabRounding always the same value as FrameRounding
	ImGui::SliderFloat("Smooth alpha", &vars::styling->smoothAlpha, 0.0f, 1.0f);
	ImGui::SliderFloat("Smooth outline", &vars::styling->smoothOutline, 0.0f, 1.0f);
	ImGui::SliderFloat("Smooth pop", &vars::styling->smoothPop, 0.0f, 1.0f);
	ImGui::SliderFloat("Speed move", &vars::styling->speedMove, 0.0f, 15.0f);
	{ bool border = (style.WindowBorderSize > 0.0f); if (ImGui::Checkbox("WindowBorder", &border)) { style.WindowBorderSize = border ? 1.0f : 0.0f; } }
	ImGui::SameLine();
	{ bool border = (style.FrameBorderSize > 0.0f);  if (ImGui::Checkbox("FrameBorder", &border)) { style.FrameBorderSize = border ? 1.0f : 0.0f; } }
	ImGui::SameLine();
	{ bool border = (style.PopupBorderSize > 0.0f);  if (ImGui::Checkbox("PopupBorder", &border)) { style.PopupBorderSize = border ? 1.0f : 0.0f; } }

	// Save/Revert button
	if (ImGui::Button("Save Ref"))
		*ref = ref_saved_style = style;
	ImGui::SameLine();
	if (ImGui::Button("Revert Ref"))
		style = *ref;
	ImGui::SameLine();
	HelpMarker(
		"Save/Revert in local non-persistent storage. Default Colors definition are not affected. "
		"Use \"Export\" below to save them somewhere.");

	ImGui::Separator();

	if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("Sizes"))
		{
			bool changedbut = false; // small optimazation for stack. Won't be huge, just less copying
			ImGui::Text("Main");
			changedbut |= ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
			changedbut |= ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
			changedbut |= ImGui::SliderFloat2("CellPadding", (float*)&style.CellPadding, 0.0f, 20.0f, "%.0f");
			changedbut |= ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
			changedbut |= ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
			changedbut |= ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
			changedbut |= ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
			changedbut |= ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
			changedbut |= ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
			ImGui::Text("Borders");
			changedbut |= ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
			changedbut |= ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
			changedbut |= ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
			changedbut |= ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 3.0f, "%.0f");
			changedbut |= ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
			ImGui::Text("Rounding");
			changedbut |= ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
			changedbut |= ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
			changedbut |= ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
			changedbut |= ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
			changedbut |= ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
			changedbut |= ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
			changedbut |= ImGui::SliderFloat("LogSliderDeadzone", &style.LogSliderDeadzone, 0.0f, 12.0f, "%.0f");
			changedbut |= ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
			ImGui::Text("Alignment");
			changedbut |= ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
			int window_menu_button_position = style.WindowMenuButtonPosition + 1;
			if (changedbut |= ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
				style.WindowMenuButtonPosition = window_menu_button_position - 1;
			changedbut |= ImGui::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
			changedbut |= ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
			ImGui::SameLine(); HelpMarker("Alignment applies when a button is larger than its text content.");
			changedbut |= ImGui::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
			ImGui::SameLine(); HelpMarker("Alignment applies when a selectable is larger than its text content.");
			ImGui::Text("Safe Area Padding");
			ImGui::SameLine(); HelpMarker("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
			changedbut |= ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
			ImGui::EndTabItem();

			if (changedbut)
			{
				// this should obvioulsy be in some struct, or just overwite varstyle = style. But then we would save unwanted stuff
				vars::styling->imStyle.WindowPadding = style.WindowPadding;
				vars::styling->imStyle.FramePadding = style.FramePadding;
				vars::styling->imStyle.CellPadding = style.CellPadding;
				vars::styling->imStyle.ItemSpacing = style.ItemSpacing;
				vars::styling->imStyle.ItemInnerSpacing = style.ItemInnerSpacing;
				vars::styling->imStyle.TouchExtraPadding = style.TouchExtraPadding;
				vars::styling->imStyle.IndentSpacing = style.IndentSpacing;
				vars::styling->imStyle.ScrollbarSize = style.ScrollbarSize;
				vars::styling->imStyle.GrabMinSize = style.GrabMinSize;
				vars::styling->imStyle.WindowBorderSize = style.WindowBorderSize;
				vars::styling->imStyle.ChildBorderSize = style.ChildBorderSize;
				vars::styling->imStyle.PopupBorderSize = style.PopupBorderSize;
				vars::styling->imStyle.FrameBorderSize = style.FrameBorderSize;
				vars::styling->imStyle.TabBorderSize = style.TabBorderSize;
				vars::styling->imStyle.WindowRounding = style.WindowRounding;
				vars::styling->imStyle.ChildRounding = style.ChildRounding;
				vars::styling->imStyle.FrameRounding = style.FrameRounding;
				vars::styling->imStyle.PopupRounding = style.PopupRounding;
				vars::styling->imStyle.ScrollbarRounding = style.ScrollbarRounding;
				vars::styling->imStyle.GrabRounding = style.GrabRounding;
				vars::styling->imStyle.LogSliderDeadzone = style.LogSliderDeadzone;
				vars::styling->imStyle.TabRounding = style.TabRounding;
				vars::styling->imStyle.WindowMenuButtonPosition = style.WindowMenuButtonPosition;
				vars::styling->imStyle.ColorButtonPosition = style.ColorButtonPosition;
				vars::styling->imStyle.ButtonTextAlign = style.ButtonTextAlign;
				vars::styling->imStyle.SelectableTextAlign = style.SelectableTextAlign;
				vars::styling->imStyle.DisplaySafeAreaPadding = style.DisplaySafeAreaPadding;
			}
		}

		if (ImGui::BeginTabItem("Colors"))
		{
			static int output_dest = 0;
			static bool output_only_modified = true;

			static ImGuiTextFilter filter;
			filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

			static ImGuiColorEditFlags alpha_flags = 0;
			if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None)) { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
			if (ImGui::RadioButton("Alpha", alpha_flags == ImGuiColorEditFlags_AlphaPreview)) { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
			if (ImGui::RadioButton("Both", alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();
			HelpMarker(
				"In the color list:\n"
				"Left-click on color square to open color picker,\n"
				"Right-click to open edit options menu.");

			ImGui::BeginChild("##colors", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
			ImGui::PushItemWidth(-160);
			for (int i = 0; i < ImGuiCol_COUNT; i++)
			{
				const char* name = ImGui::GetStyleColorName(i);
				if (!filter.PassFilter(name))
					continue;
				ImGui::PushID(i);
				ImGui::ColorEdit4("##color", (float*)&style.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
				if (memcmp(&style.Colors[i], &ref->Colors[i], sizeof(ImVec4)) != 0)
				{
					// Tips: in a real user application, you may want to merge and use an icon font into the main font,
					// so instead of "Save"/"Revert" you'd use icons!
					// Read the FAQ and docs/FONTS.md about using icon fonts. It's really easy and super convenient!
					ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save"))
					{
						ref->Colors[i] = style.Colors[i];
						vars::styling->imStyle.Colors[i] = style.Colors[i];
					}
					ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { style.Colors[i] = ref->Colors[i]; }
				}
				ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
				ImGui::TextUnformatted(name);
				ImGui::PopID();
			}

			// should fix this... by own struct
			for (size_t i = 0; i < vars::styling->groupPanelBackground.size(); i++)
			{
				constexpr std::array names = { "Groupleft_up", "Groupright_up", "Groupright_down", "Groupleft_down" };
				const char* name = names[i];
				if (!filter.PassFilter(name))
					continue;
				ImGui::PushID(names[i]);

				ImGui::ColorEdit4("##color2", vars::styling->groupPanelBackground[i].getColorRef().data(), ImGuiColorEditFlags_AlphaBar | alpha_flags);
				if (memcmp(vars::styling->groupPanelBackground[i].getColorRef().data(), ref_saved_gradient[i].getColorRef().data(), sizeof(ImVec4)) != 0)
				{
					// Tips: in a real user application, you may want to merge and use an icon font into the main font,
					// so instead of "Save"/"Revert" you'd use icons!
					// Read the FAQ and docs/FONTS.md about using icon fonts. It's really easy and super convenient!
					ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save"))
					{
						ref_saved_gradient[i] = vars::styling->groupPanelBackground[i];
					}
					ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { vars::styling->groupPanelBackground[i] = ref_saved_gradient[i]; }
				}
				ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
				ImGui::TextUnformatted(name);
				ImGui::PopID();
			}

			const char* name = "Groupline";
			if (filter.PassFilter(name))
			{
				ImGui::PushID(name);
				ImGui::ColorEdit4("##color3", vars::styling->groupPanelLines.getColorRef().data(), ImGuiColorEditFlags_AlphaBar | alpha_flags);

				if (memcmp(vars::styling->groupPanelLines.getColorRef().data(), ref_saved_color_panel_line.getColorRef().data(), sizeof(ImVec4)) != 0)
				{
					// Tips: in a real user application, you may want to merge and use an icon font into the main font,
					// so instead of "Save"/"Revert" you'd use icons!
					// Read the FAQ and docs/FONTS.md about using icon fonts. It's really easy and super convenient!
					ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Save"))
					{
						ref_saved_color_panel_line = vars::styling->groupPanelLines;
					}
					ImGui::SameLine(0.0f, style.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { vars::styling->groupPanelLines = ref_saved_color_panel_line; }
				}

				ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
				ImGui::TextUnformatted(name);
				ImGui::PopID();
			}

			ImGui::PopItemWidth();
			ImGui::EndChild();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Fonts"))
		{
			ImGuiIO& io = ImGui::GetIO();
			ImFontAtlas* atlas = io.Fonts;
			HelpMarker("Read FAQ and docs/FONTS.md for details on font loading.");
			ImGui::ShowFontAtlas(atlas);

			// Post-baking font scaling. Note that this is NOT the nice way of scaling fonts, read below.
			// (we enforce hard clamping manually as by default DragFloat/SliderFloat allows CTRL+Click text to get out of bounds).
			const float MIN_SCALE = 0.3f;
			const float MAX_SCALE = 2.0f;
			HelpMarker(
				"Those are old settings provided for convenience.\n"
				"However, the _correct_ way of scaling your UI is currently to reload your font at the designed size, "
				"rebuild the font atlas, and call style.ScaleAllSizes() on a reference ImGuiStyle structure.\n"
				"Using those settings here will give you poor quality results.");
			static float window_scale = 1.0f;
			ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
			if (ImGui::DragFloat("window scale", &window_scale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_AlwaysClamp)) // Scale only this window
				ImGui::SetWindowFontScale(window_scale);
			ImGui::DragFloat("global scale", &io.FontGlobalScale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_AlwaysClamp); // Scale everything
			ImGui::PopItemWidth();

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Rendering"))
		{
			ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
			ImGui::SameLine();
			HelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");

			ImGui::Checkbox("Anti-aliased lines use texture", &style.AntiAliasedLinesUseTex);
			ImGui::SameLine();
			HelpMarker("Faster lines using texture data. Require backend to render with bilinear filtering (not point/nearest filtering).");

			ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
			ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
			ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
			if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;

			// When editing the "Circle Segment Max Error" value, draw a preview of its effect on auto-tessellated circles.
			ImGui::DragFloat("Circle Tessellation Max Error", &style.CircleTessellationMaxError, 0.005f, 0.10f, 5.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
			if (ImGui::IsItemActive())
			{
				ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
				ImGui::BeginTooltip();
				ImGui::TextUnformatted("(R = radius, N = number of segments)");
				ImGui::Spacing();
				ImDrawList* draw_list = ImGui::GetWindowDrawList();
				const float min_widget_width = ImGui::CalcTextSize("N: MMM\nR: MMM").x;
				for (int n = 0; n < 8; n++)
				{
					const float RAD_MIN = 5.0f;
					const float RAD_MAX = 70.0f;
					const float rad = RAD_MIN + (RAD_MAX - RAD_MIN) * (float)n / (8.0f - 1.0f);

					ImGui::BeginGroup();

					ImGui::Text("R: %.f\nN: %d", rad, draw_list->_CalcCircleAutoSegmentCount(rad));

					const float canvas_width = ImMax(min_widget_width, rad * 2.0f);
					const float offset_x = floorf(canvas_width * 0.5f);
					const float offset_y = floorf(RAD_MAX);

					const ImVec2 p1 = ImGui::GetCursorScreenPos();
					draw_list->AddCircle(ImVec2(p1.x + offset_x, p1.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
					ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));

					/*
					const ImVec2 p2 = ImGui::GetCursorScreenPos();
					draw_list->AddCircleFilled(ImVec2(p2.x + offset_x, p2.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
					ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));
					*/

					ImGui::EndGroup();
					ImGui::SameLine();
				}
				ImGui::EndTooltip();
			}
			ImGui::SameLine();
			HelpMarker("When drawing circle primitives with \"num_segments == 0\" tesselation will be calculated automatically.");

			ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
			ImGui::DragFloat("Disabled Alpha", &style.DisabledAlpha, 0.005f, 0.0f, 1.0f, "%.2f"); ImGui::SameLine(); HelpMarker("Additional alpha multiplier for disabled items (multiply over current value of Alpha).");
			ImGui::PopItemWidth();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::PopItemWidth();
}

void ImGui::LoadCustomSettings()
{
	ImGuiContext& g = *GImGui;
	for (ImGuiWindowSettings* settings = g.SettingsWindows.begin(); settings != NULL; settings = g.SettingsWindows.next_chunk(settings))
	{
		const ImVec2 pos = ImVec2{ static_cast<float>(settings->Pos.x), static_cast<float>(settings->Pos.y) };
		const ImVec2 size = ImVec2{ static_cast<float>(settings->Size.x), static_cast<float>(settings->Size.y) };

		//std::cout << "name : " << settings->GetName() << " Size " << size.x << " " << size.y << '\n';
		
		extraGlobals::settings[ImGui::GetID(settings->GetName())] =
			ImWindowSettings
		{
			.id = settings->ID,
			.pos = pos + (size / 2),
			.size = ImVec2{},
			.targetSize = size,
			.alpha = 0.0f
		};
	}
}

#include <d3d9.h>

#ifdef IMGUI_USE_BGRA_PACKED_COLOR
#define IMGUI_COL_TO_DX9_ARGB(_COL)     (_COL)
#else
#define IMGUI_COL_TO_DX9_ARGB(_COL)     (((_COL) & 0xFF00FF00) | (((_COL) & 0xFF0000) >> 16) | (((_COL) & 0xFF) << 16))
#endif

struct ImGui_ImplDX9_Data
{
	LPDIRECT3DDEVICE9           pd3dDevice;
	LPDIRECT3DVERTEXBUFFER9     pVB;
	LPDIRECT3DINDEXBUFFER9      pIB;
	LPDIRECT3DTEXTURE9          FontTexture;
	int                         VertexBufferSize;
	int                         IndexBufferSize;

	ImGui_ImplDX9_Data() { memset(this, 0, sizeof(*this)); VertexBufferSize = 5000; IndexBufferSize = 10000; }
};

static ImGui_ImplDX9_Data* ImGui_ImplDX9_GetBackendData()
{
	return ImGui::GetCurrentContext() ? (ImGui_ImplDX9_Data*)ImGui::GetIO().BackendRendererUserData : NULL;
}

void* ImGui_CreateTexture(const void* data, int width, int height)
{
	ImGui_ImplDX9_Data* bd = ImGui_ImplDX9_GetBackendData();
	unsigned char* pixels;

	if (!bd || !bd->pd3dDevice || !data)
		return NULL;

	// Convert RGBA32 to BGRA32 (because RGBA32 is not well supported by DX9 devices)
#ifndef IMGUI_USE_BGRA_PACKED_COLOR
	ImU32* dst_start = (ImU32*)ImGui::MemAlloc((size_t)width * height * 4);
	for (ImU32* src = (ImU32*)data, *dst = dst_start, *dst_end = dst_start + (size_t)width * height; dst < dst_end; src++, dst++)
		*dst = IMGUI_COL_TO_DX9_ARGB(*src);
	pixels = (unsigned char*)dst_start;
#endif

	LPDIRECT3DTEXTURE9 temp;
	if (bd->pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_SYSTEMMEM, &temp, NULL) < 0)
		return NULL;
	D3DLOCKED_RECT tex_locked_rect;
	if (temp->LockRect(0, &tex_locked_rect, NULL, D3DLOCK_DISCARD) != D3D_OK)
	{
		temp->Release();
		return NULL;
	}
	for (int y = 0; y < height; ++y)
		memcpy((unsigned char*)tex_locked_rect.pBits + tex_locked_rect.Pitch * y, pixels + (width * 4) * y, (width * 4));
	temp->UnlockRect(0);

	LPDIRECT3DTEXTURE9 texture;
	if (bd->pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL) != D3D_OK)
	{
		temp->Release();
		return NULL;
	}
	bd->pd3dDevice->UpdateTexture(temp, texture);
	temp->Release();

#ifndef IMGUI_USE_BGRA_PACKED_COLOR
	ImGui::MemFree(pixels);
#endif

	return texture;
}

void ImGui_DestroyTexture(void* texture)
{
	IM_ASSERT(texture != NULL && "passed texture to destroy was null!");
	reinterpret_cast<IDirect3DTexture9*>(texture)->Release();
}