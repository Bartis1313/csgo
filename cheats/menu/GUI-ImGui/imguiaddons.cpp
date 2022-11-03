#include "imguiaddons.hpp"

#include <config/config.hpp>
#include <utilities/utilities.hpp>
#include <game/globals.hpp>

void ImGui::Hotkey(const char* label, Key* key, bool useExtended, const ImVec2& size)
{
	ImGui::PushID(label);
	if (std::strncmp(label, "##", 2))
		ImGui::TextUnformatted(label, std::strstr(label, "##"));

	ImGui::SameLine();

	if (const auto id = ImGui::GetID(label); ImGui::GetActiveID() == id)
	{
		ImGui::Button("...", size);
		ImGui::GetCurrentContext()->ActiveIdAllowOverlap = true;

		if ((!ImGui::IsItemHovered() && ImGui::GetIO().MouseClicked[0]) || key->checkKey())
		{
			globals::isInHotkey = false;
			ImGui::ClearActiveID();
		}

	}
	else if (ImGui::Button(utilities::getKeyName(key->getKeyCode()).c_str(), size))
	{
		globals::isInHotkey = true;
		ImGui::SetActiveID(id, GetCurrentWindow());
	}
	else
	{
		if (useExtended)
		{
			if (ImGui::BeginPopup("##pop", ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
			{
				for (const auto [mode, name] : Key::getKeyPairs())
				{
					bool selected = key->getKeyMode() == mode;
					if (ImGui::Selectable(name, &selected))
					{
						if (selected)
							key->setKeyMode(mode);
					}
				}

				ImGui::EndPopup();
			}
			else if (IsItemHovered())
			{
				ImGui::SetTooltip("Key mode");

				if (ImGui::GetIO().MouseClicked[1])
					ImGui::OpenPopup("##pop");
			}
		}
	}

	ImGui::PopID();
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

bool ImGui::ColorPicker(const char* label, CfgColor* clr)
{
	constexpr auto pickerFlags = ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar;
	constexpr auto pickerButtonFlags = ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_AlphaPreview;
	constexpr auto paletteButoonFlags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;

	bool toReturn = false;

	static std::array<Color, 32> palette = {};
	static auto bOnce = [=]()
	{
		for (size_t i = 0; auto & el : palette)
		{
			ImGui::ColorConvertHSVtoRGB(i / static_cast<float>(palette.size()), 1.0f, 1.0f,
				el.at(0), el.at(1), el.at(2));
			el.at(3) = 1.0f;

			i++;
		}
		return true;
	} ();

	ImGui::PushID(label);

	bool openPopup = ImGui::ColorButton("##colbut", ImVec4{ clr->getColor().r(), clr->getColor().g(), clr->getColor().b(), clr->getColor().a() }, pickerButtonFlags);

	if (std::strncmp(label, "##", 2))
	{
		ImGui::SameLine();
		ImGui::TextUnformatted(label, std::strstr(label, "##"));
	}

	if (openPopup)
		ImGui::OpenPopup("##colpop");

	if (ImGui::BeginPopup("##colpop"))
	{
		std::array col{ clr->getColor().r(), clr->getColor().g(), clr->getColor().b(), clr->getColor().a() };
		toReturn = ImGui::ColorPicker4("##colpicker", col.data(), pickerFlags);
		*clr = CfgColor{ col, clr->getRainbow(), clr->getSpeed() };

		ImGui::SameLine();

		constexpr int seperateLimit = 7;
		constexpr auto paletteButtonSize = ImVec2{ 20.0f, 20.0f };

		if (ImGui::BeginChild("##colorsaddon", { seperateLimit * paletteButtonSize.x, 0.0f }))
		{
			for (size_t i = 0; const auto & el : palette)
			{
				ImGui::PushID(i);
				if ((i % seperateLimit) != 0)
					ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);

				if (ImGui::ColorButton("##palette", ImVec4{ el.r(), el.g(), el.b(), el.a() }, paletteButoonFlags, paletteButtonSize))
				{
					*clr = CfgColor{ Color{ el, clr->getColor().a() }, clr->getRainbow(), clr->getSpeed() };
					toReturn = true;
				}
				ImGui::PopID();

				i++;
			}

			ImGui::Checkbox("Rainbow mode", &clr->getRainbowRef());
			if (clr->getRainbow())
			{
				ImGui::PushItemWidth(seperateLimit * paletteButtonSize.x);
				ImGui::SliderFloat("##ranbowspeed", &clr->getSpeedRef(), 0.0f, 15.0f, "Speed %.1f", ImGuiSliderFlags_Logarithmic);
				ImGui::PopItemWidth();

				// apply return as updated color
				toReturn = true;
			}

			ImGui::EndChild();
		}

		ImGui::EndPopup();

	}
	ImGui::PopID();

	return toReturn;
}

#include <config/vars.hpp>
#include <unordered_map>

static std::unordered_map<ImGuiID, std::pair<ImVec2, ImVec2>> m_mapSizes = {};
static const char* g_GroupPanelName = nullptr;
static ImVector<ImRect> s_GroupPanelLabelStack;

void ImGui::BeginGroupPanel(const char* name, const ImVec2& size)
{
	g_GroupPanelName = name;

	auto id = ImGui::GetCurrentWindow()->GetID(name);
	auto [min, max] = m_mapSizes[id];

	ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
		min, max,
		Color::U32(vars::styling->groupPanelBackground[0]()),
		Color::U32(vars::styling->groupPanelBackground[1]()),
		Color::U32(vars::styling->groupPanelBackground[2]()),
		Color::U32(vars::styling->groupPanelBackground[3]())
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
	ImGui::TextUnformatted(name);
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

#include <utilities/renderer/renderer.hpp>

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
			halfFrame.x);

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

bool ImGui::PopupButton(const char* label, const std::function<void()>& fun)
{
	ImGui::PushID(label);

	if (ImGui::Button("Options"))
		ImGui::OpenPopup("");

	bool ret = false;
	if (ImGui::BeginPopup(""))
	{
		fun();
		ret = true;

		ImGui::EndPopup();
	}

	ImGui::PopID();

	return ret;
}


ImGui::ExampleAppLog::ExampleAppLog()
{
	AutoScroll = true;
	Clear();
}

void ImGui::ExampleAppLog::Clear()
{
	Buf.clear();
	LineOffsets.clear();
	LineOffsets.push_back(0);
}

void ImGui::ExampleAppLog::Draw(const char* title, bool* p_open)
{
	if (!ImGui::Begin(title, p_open))
	{
		ImGui::End();
		return;
	}

	// Options menu
	if (ImGui::BeginPopup("Options"))
	{
		ImGui::Checkbox("Auto-scroll", &AutoScroll);
		ImGui::EndPopup();
	}

	// Main window
	if (ImGui::Button("Options"))
		ImGui::OpenPopup("Options");
	ImGui::SameLine();
	bool clear = ImGui::Button("Clear");
	ImGui::SameLine();
	bool copy = ImGui::Button("Copy");
	ImGui::SameLine();
	Filter.Draw("Filter", -100.0f);

	ImGui::Separator();
	ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

	if (clear)
		Clear();
	if (copy)
		ImGui::LogToClipboard();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	const char* buf = Buf.begin();
	const char* buf_end = Buf.end();
	if (Filter.IsActive())
	{
		// In this example we don't use the clipper when Filter is enabled.
		// This is because we don't have a random access on the result on our filter.
		// A real application processing logs with ten of thousands of entries may want to store the result of
		// search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
		for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
		{
			const char* line_start = buf + LineOffsets[line_no];
			const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
			if (Filter.PassFilter(line_start, line_end))
				ImGui::TextUnformatted(line_start, line_end);
		}
	}
	else
	{
		// The simplest and easy way to display the entire buffer:
		//   ImGui::TextUnformatted(buf_begin, buf_end);
		// And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
		// to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
		// within the visible area.
		// If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
		// on your side is recommended. Using ImGuiListClipper requires
		// - A) random access into your data
		// - B) items all being the  same height,
		// both of which we can handle since we an array pointing to the beginning of each line of text.
		// When using the filter (in the block of code above) we don't have random access into the data to display
		// anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
		// it possible (and would be recommended if you want to search through tens of thousands of entries).
		ImGuiListClipper clipper;
		clipper.Begin(LineOffsets.Size);
		while (clipper.Step())
		{
			for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
			{
				const char* line_start = buf + LineOffsets[line_no];
				const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
				ImGui::TextUnformatted(line_start, line_end);
			}
		}
		clipper.End();
	}
	ImGui::PopStyleVar();

	if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);

	ImGui::EndChild();
	ImGui::End();
}

void ImGui::ExampleAppLog::AddLog(const std::string& buffer)
{
	if (buffer.empty())
		return;

	int oldSize = Buf.size();

	Buf.append(buffer.c_str());

	for (int newSize = Buf.size(); oldSize < newSize; oldSize++)
		if (Buf[oldSize] == '\n')
			LineOffsets.push_back(oldSize + 1);
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

void ImGui::Animated::RenderCheckMarkExtra(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float sz, float thickness_extra)
{
	float thickness = ImMax(sz * thickness_extra / 5.0f, 1.0f) * thickness_extra;
	sz -= thickness * 0.5f;
	pos += ImVec2(thickness * 0.25f, thickness * 0.25f);

	float third = sz / 3.0f;
	float bx = pos.x + third;
	float by = pos.y + sz - third * 0.5f;
	draw_list->PathLineTo(ImVec2(bx - third, by - third));
	draw_list->PathLineTo(ImVec2(bx, by));
	draw_list->PathLineTo(ImVec2(bx + third * 2.0f, by - third * 2.0f));
	draw_list->PathStroke(col, 0, thickness);
}

void ImGui::Animated::RenderFrameAlpha(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding, float alpha)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	ImColor color(fill_col);
	color.Value.w = alpha;
	window->DrawList->AddRectFilled(p_min, p_max, color, rounding);
	const float border_size = g.Style.FrameBorderSize;
	if (border && border_size > 0.0f)
	{
		window->DrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1), GetColorU32(ImGuiCol_BorderShadow, alpha), rounding, 0, border_size);
		window->DrawList->AddRect(p_min, p_max, GetColorU32(ImGuiCol_Border, alpha), rounding, 0, border_size);
	}
}

void ImGui::Animated::RenderFrameAlphaOutline(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding, float alpha, float outline)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	ImColor color(fill_col);
	color.Value.w = alpha;
	window->DrawList->AddRectFilled(p_min, p_max, color, rounding);
	const float border_size = g.Style.FrameBorderSize;
	if (border && border_size > 0.0f)
	{
		window->DrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1), GetColorU32(ImGuiCol_BorderShadow, outline), rounding, 0, border_size);
		window->DrawList->AddRect(p_min, p_max, GetColorU32(ImGuiCol_Border, outline), rounding, 0, border_size);
	}
}


void ImGui::Animated::RenderFrameOutlineKeepFill(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding, float outline)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	window->DrawList->AddRectFilled(p_min, p_max, fill_col, rounding);
	const float border_size = g.Style.FrameBorderSize;
	if (border && border_size > 0.0f)
	{
		window->DrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1), GetColorU32(ImGuiCol_BorderShadow, outline), rounding, 0, border_size);
		window->DrawList->AddRect(p_min, p_max, GetColorU32(ImGuiCol_Border, outline), rounding, 0, border_size);
	}
}

void ImGui::Animated::RenderTextAlpha(ImVec2 pos, const char* text, const char* text_end, bool hide_text_after_hash, float alpha)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;

	// Hide anything after a '##' string
	const char* text_display_end;
	if (hide_text_after_hash)
	{
		text_display_end = FindRenderedTextEnd(text, text_end);
	}
	else
	{
		if (!text_end)
			text_end = text + strlen(text); // FIXME-OPT
		text_display_end = text_end;
	}

	if (text != text_display_end)
	{
		window->DrawList->AddText(g.Font, g.FontSize, pos, GetColorU32(ImGuiCol_Text, alpha), text, text_display_end);
		if (g.LogEnabled)
			LogRenderedText(&pos, text, text_display_end);
	}
}

void ImGui::Animated::RenderFrameBorderAlpha(ImVec2 p_min, ImVec2 p_max, float rounding, float alpha)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	const float border_size = g.Style.FrameBorderSize;
	if (border_size > 0.0f)
	{
		window->DrawList->AddRect(p_min + ImVec2(1, 1), p_max + ImVec2(1, 1), GetColorU32(ImGuiCol_BorderShadow, alpha), rounding, 0, border_size);
		window->DrawList->AddRect(p_min, p_max, GetColorU32(ImGuiCol_Border, alpha), rounding, 0, border_size);
	}
}

bool ImGui::Animated::Checkbox(const char* label, bool* v)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	const float square_sz = GetFrameHeight();
	const ImVec2 pos = window->DC.CursorPos;
	const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
	ItemSize(total_bb, style.FramePadding.y);
	if (!ItemAdd(total_bb, id))
	{
		IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
		return false;
	}

	bool hovered, held;
	bool pressed = ButtonBehavior(total_bb, id, &hovered, &held);
	if (pressed)
	{
		*v = !(*v);
		MarkItemEdited(id);
	}

	static std::unordered_map<ImGuiID, CheckboxAnim> anim;
	auto map_itr = anim.find(id);

	auto pair = (map_itr != anim.end()) ? map_itr->second : CheckboxAnim{};
	auto alpha = pair.alpha;
	auto thickness = pair.thickness;
	auto checkmark = pair.checkMark;
	auto lastTime = pair.lastTime;
	auto outline = pair.alpha_outline;

	const float ANIM_SPEED = 1.0f; // bigger values mean faster change
	const float ANIM_SPEED_CHECKBOX = 10.0f; // same as ^
	const float deltatime = g.IO.DeltaTime * ANIM_SPEED;

	// don't use global timers, they exist and may work well
	// however very slow animation while changing active ID will reset the timer, while animation didnt end
	// so we handle it on own in ID map
	if (held || pressed)
		lastTime = 0.0f;

	if (lastTime == 0.f && !pressed)
		lastTime = g.LastActiveIdTimer; // use global timer, this time it won't matter

	lastTime += g.IO.DeltaTime;

	const float temp_thickness = ImSaturate(lastTime * ANIM_SPEED_CHECKBOX); // magic number, looks nice
	thickness = held ? temp_thickness : (1.0f - temp_thickness);
	checkmark = *v ? temp_thickness : 0.0f;

	const ImColor frame_col = ImColor(GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg));
	const float limitAlpha = frame_col.Value.w;
	const float limitAlphaOutline = hovered ? 1.0f : 0.4f; // breaking on lower alphas...

	if (alpha < limitAlpha)
		alpha = ImMin(alpha + deltatime, limitAlpha);
	else if (alpha > limitAlpha)
		alpha = ImMax(alpha - deltatime, limitAlpha);

	if (outline < limitAlphaOutline)
		outline = ImMin(outline + deltatime, limitAlphaOutline);
	else if (outline > limitAlphaOutline)
		outline = ImMax(outline - deltatime, limitAlphaOutline);

	if (map_itr != anim.end())
		map_itr->second = CheckboxAnim{ alpha, thickness, checkmark, lastTime, outline };
	else
		anim.emplace(std::make_pair(id, CheckboxAnim{ alpha, thickness,checkmark, lastTime, outline }));

	const float size_frame_extra = 3.f * thickness;
	ImVec2 extra_size = ImVec2(size_frame_extra, size_frame_extra);

	const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
	const ImRect frame_bb(ImVec2(pos + extra_size), pos + ImVec2(square_sz, square_sz) - extra_size);
	RenderNavHighlight(total_bb, id);
	RenderFrameAlphaOutline(frame_bb.Min, frame_bb.Max, frame_col, true, style.FrameRounding, alpha, outline);

	ImU32 check_col = GetColorU32(ImGuiCol_CheckMark);
	bool mixed_value = (g.LastItemData.InFlags & ImGuiItemFlags_MixedValue) != 0;
	if (mixed_value)
	{
		// Undocumented tristate/mixed/indeterminate checkbox (#2644)
		// This may seem awkwardly designed because the aim is to make ImGuiItemFlags_MixedValue supported by all widgets (not just checkbox)
		ImVec2 pad(ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)), ImMax(1.0f, IM_FLOOR(square_sz / 3.6f)));
		window->DrawList->AddRectFilled(check_bb.Min + pad, check_bb.Max - pad, check_col, style.FrameRounding);
	}
	else if (*v || checkmark > 0.0f)
	{
		const float pad = ImMax(1.0f, IM_FLOOR(square_sz / 6.0f));
		RenderCheckMarkExtra(window->DrawList, check_bb.Min + ImVec2(pad, pad), check_col, square_sz - pad * 2.0f, checkmark);
	}

	ImVec2 label_pos = ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y);
	if (g.LogEnabled)
		LogRenderedText(&label_pos, mixed_value ? "[~]" : *v ? "[x]" : "[ ]");
	if (label_size.x > 0.0f)
		RenderText(label_pos, label);

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
	return pressed;
}

bool ImGui::Animated::BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = GetCurrentWindow();

	ImGuiNextWindowDataFlags backup_next_window_data_flags = g.NextWindowData.Flags;
	g.NextWindowData.ClearFlags(); // We behave like Begin() and need to consume those values
	if (window->SkipItems)
		return false;

	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	IM_ASSERT((flags & (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)) != (ImGuiComboFlags_NoArrowButton | ImGuiComboFlags_NoPreview)); // Can't use both flags together

	const float arrow_size = (flags & ImGuiComboFlags_NoArrowButton) ? 0.0f : GetFrameHeight();
	const ImVec2 label_size = CalcTextSize(label, NULL, true);
	const float w = (flags & ImGuiComboFlags_NoPreview) ? arrow_size : CalcItemWidth();
	const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
	const ImRect total_bb(bb.Min, bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));
	ItemSize(total_bb, style.FramePadding.y);
	if (!ItemAdd(total_bb, id, &bb))
		return false;

	// Open on click
	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held);
	const ImGuiID popup_id = ImHashStr("##ComboPopup", 0, id);
	bool popup_open = IsPopupOpen(popup_id, ImGuiPopupFlags_None);
	if (pressed && !popup_open)
	{
		OpenPopupEx(popup_id, ImGuiPopupFlags_None);
		popup_open = true;
	}

	static std::unordered_map<ImGuiID, AlphaAnimOutline> anim;
	auto map_itr = anim.find(id);

	auto pair = (map_itr != anim.end()) ? map_itr->second : AlphaAnimOutline{};
	auto alpha = pair.alpha;
	auto outline = pair.alpha_outline;

	const float limitAlpha = ImColor(GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg)).Value.w;
	const float limitAlphaOutline = hovered || popup_open ? 1.0f : 0.4f; // breaking on lower alphas...

	const float ANIM_SPEED = 1.0f; // bigger values mean faster change
	const float ANIM_SPEED_CHECKBOX = 10.0f; // same as ^
	const float deltatime = g.IO.DeltaTime * ANIM_SPEED;

	if (alpha < limitAlpha)
		alpha = ImMin(alpha + deltatime, limitAlpha);
	else if (alpha > limitAlpha)
		alpha = ImMax(alpha - deltatime, limitAlpha);

	if (outline < limitAlphaOutline)
		outline = ImMin(outline + deltatime, limitAlphaOutline);
	else if (outline > limitAlphaOutline)
		outline = ImMax(outline - deltatime, limitAlphaOutline);

	if (map_itr != anim.end())
		map_itr->second = AlphaAnimOutline{ alpha, outline };
	else
		anim.emplace(std::make_pair(id, AlphaAnimOutline{ alpha, outline }));

	// Render shape
	const ImU32 frame_col = GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
	const float value_x2 = ImMax(bb.Min.x, bb.Max.x - arrow_size);
	RenderNavHighlight(bb, id);
	if (!(flags & ImGuiComboFlags_NoPreview))
		window->DrawList->AddRectFilled(bb.Min, ImVec2(ImMax(bb.Min.x, bb.Max.x - 1.0f), bb.Max.y), frame_col, style.FrameRounding);
	if (!(flags & ImGuiComboFlags_NoArrowButton))
	{
		//ImU32 bg_col = GetColorU32((popup_open || hovered) ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		ImU32 text_col = GetColorU32(ImGuiCol_Text, alpha);
		//window->DrawList->AddRectFilled(ImVec2(value_x2, bb.Min.y), bb.Max, bg_col, style.FrameRounding, (w <= arrow_size) ? ImDrawFlags_RoundCornersAll : ImDrawFlags_RoundCornersRight);
		if (value_x2 + arrow_size - style.FramePadding.x <= bb.Max.x)
			RenderArrow(window->DrawList, ImVec2(value_x2 + style.FramePadding.y, bb.Min.y + style.FramePadding.y), text_col, ImGuiDir_Down, 1.0f);
	}
	RenderFrameBorderAlpha(bb.Min, bb.Max, style.FrameRounding, outline);

	// Custom preview
	if (flags & ImGuiComboFlags_CustomPreview)
	{
		g.ComboPreviewData.PreviewRect = ImRect(bb.Min.x, bb.Min.y, value_x2, bb.Max.y);
		IM_ASSERT(preview_value == NULL || preview_value[0] == 0);
		preview_value = NULL;
	}

	// Render preview and label
	if (preview_value != NULL && !(flags & ImGuiComboFlags_NoPreview))
	{
		if (g.LogEnabled)
			LogSetNextTextDecoration("{", "}");
		RenderTextClipped(bb.Min + style.FramePadding, ImVec2(value_x2, bb.Max.y), preview_value, NULL, NULL);
	}
	if (label_size.x > 0)
		RenderText(ImVec2(bb.Max.x + style.ItemInnerSpacing.x, bb.Min.y + style.FramePadding.y), label);

	if (!popup_open)
		return false;

	g.NextWindowData.Flags = backup_next_window_data_flags;
	return BeginComboPopup(popup_id, bb, flags);
}

bool ImGui::Animated::Selectable(const char* label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size_arg)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;

	// Submit label or explicit size to ItemSize(), whereas ItemAdd() will submit a larger/spanning rectangle.
	ImGuiID id = window->GetID(label);
	ImVec2 label_size = CalcTextSize(label, NULL, true);
	ImVec2 size(size_arg.x != 0.0f ? size_arg.x : label_size.x, size_arg.y != 0.0f ? size_arg.y : label_size.y);
	ImVec2 pos = window->DC.CursorPos;
	pos.y += window->DC.CurrLineTextBaseOffset;
	ItemSize(size, 0.0f);

	// Fill horizontal space
	// We don't support (size < 0.0f) in Selectable() because the ItemSpacing extension would make explicitly right-aligned sizes not visibly match other widgets.
	const bool span_all_columns = (flags & ImGuiSelectableFlags_SpanAllColumns) != 0;
	const float min_x = span_all_columns ? window->ParentWorkRect.Min.x : pos.x;
	const float max_x = span_all_columns ? window->ParentWorkRect.Max.x : window->WorkRect.Max.x;
	if (size_arg.x == 0.0f || (flags & ImGuiSelectableFlags_SpanAvailWidth))
		size.x = ImMax(label_size.x, max_x - min_x);

	// Text stays at the submission position, but bounding box may be extended on both sides
	const ImVec2 text_min = pos;
	const ImVec2 text_max(min_x + size.x, pos.y + size.y);

	// Selectables are meant to be tightly packed together with no click-gap, so we extend their box to cover spacing between selectable.
	ImRect bb(min_x, pos.y, text_max.x, text_max.y);
	if ((flags & ImGuiSelectableFlags_NoPadWithHalfSpacing) == 0)
	{
		const float spacing_x = span_all_columns ? 0.0f : style.ItemSpacing.x;
		const float spacing_y = style.ItemSpacing.y;
		const float spacing_L = IM_FLOOR(spacing_x * 0.50f);
		const float spacing_U = IM_FLOOR(spacing_y * 0.50f);
		bb.Min.x -= spacing_L;
		bb.Min.y -= spacing_U;
		bb.Max.x += (spacing_x - spacing_L);
		bb.Max.y += (spacing_y - spacing_U);
	}
	//if (g.IO.KeyCtrl) { GetForegroundDrawList()->AddRect(bb.Min, bb.Max, IM_COL32(0, 255, 0, 255)); }

	// Modify ClipRect for the ItemAdd(), faster than doing a PushColumnsBackground/PushTableBackground for every Selectable..
	const float backup_clip_rect_min_x = window->ClipRect.Min.x;
	const float backup_clip_rect_max_x = window->ClipRect.Max.x;
	if (span_all_columns)
	{
		window->ClipRect.Min.x = window->ParentWorkRect.Min.x;
		window->ClipRect.Max.x = window->ParentWorkRect.Max.x;
	}

	const bool disabled_item = (flags & ImGuiSelectableFlags_Disabled) != 0;
	const bool item_add = ItemAdd(bb, id, NULL, disabled_item ? ImGuiItemFlags_Disabled : ImGuiItemFlags_None);
	if (span_all_columns)
	{
		window->ClipRect.Min.x = backup_clip_rect_min_x;
		window->ClipRect.Max.x = backup_clip_rect_max_x;
	}

	if (!item_add)
		return false;

	const bool disabled_global = (g.CurrentItemFlags & ImGuiItemFlags_Disabled) != 0;
	if (disabled_item && !disabled_global) // Only testing this as an optimization
		BeginDisabled();

	// FIXME: We can standardize the behavior of those two, we could also keep the fast path of override ClipRect + full push on render only,
	// which would be advantageous since most selectable are not selected.
	if (span_all_columns && window->DC.CurrentColumns)
		PushColumnsBackground();
	else if (span_all_columns && g.CurrentTable)
		TablePushBackgroundChannel();

	// We use NoHoldingActiveID on menus so user can click and _hold_ on a menu then drag to browse child entries
	ImGuiButtonFlags button_flags = 0;
	if (flags & ImGuiSelectableFlags_NoHoldingActiveID) { button_flags |= ImGuiButtonFlags_NoHoldingActiveId; }
	if (flags & ImGuiSelectableFlags_SelectOnClick) { button_flags |= ImGuiButtonFlags_PressedOnClick; }
	if (flags & ImGuiSelectableFlags_SelectOnRelease) { button_flags |= ImGuiButtonFlags_PressedOnRelease; }
	if (flags & ImGuiSelectableFlags_AllowDoubleClick) { button_flags |= ImGuiButtonFlags_PressedOnClickRelease | ImGuiButtonFlags_PressedOnDoubleClick; }
	if (flags & ImGuiSelectableFlags_AllowItemOverlap) { button_flags |= ImGuiButtonFlags_AllowItemOverlap; }

	const bool was_selected = selected;
	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, button_flags);

	// Auto-select when moved into
	// - This will be more fully fleshed in the range-select branch
	// - This is not exposed as it won't nicely work with some user side handling of shift/control
	// - We cannot do 'if (g.NavJustMovedToId != id) { selected = false; pressed = was_selected; }' for two reasons
	//   - (1) it would require focus scope to be set, need exposing PushFocusScope() or equivalent (e.g. BeginSelection() calling PushFocusScope())
	//   - (2) usage will fail with clipped items
	//   The multi-select API aim to fix those issues, e.g. may be replaced with a BeginSelection() API.
	if ((flags & ImGuiSelectableFlags_SelectOnNav) && g.NavJustMovedToId != 0 && g.NavJustMovedToFocusScopeId == window->DC.NavFocusScopeIdCurrent)
		if (g.NavJustMovedToId == id)
			selected = pressed = true;

	// Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
	if (pressed || (hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
	{
		if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
		{
			SetNavID(id, window->DC.NavLayerCurrent, window->DC.NavFocusScopeIdCurrent, WindowRectAbsToRel(window, bb)); // (bb == NavRect)
			g.NavDisableHighlight = true;
		}
	}
	if (pressed)
		MarkItemEdited(id);

	if (flags & ImGuiSelectableFlags_AllowItemOverlap)
		SetItemAllowOverlap();

	// In this branch, Selectable() cannot toggle the selection so this will never trigger.
	if (selected != was_selected) //-V547
		g.LastItemData.StatusFlags |= ImGuiItemStatusFlags_ToggledSelection;

	// Render
	if (held && (flags & ImGuiSelectableFlags_DrawHoveredWhenHeld))
		hovered = true;
	/*if (hovered || selected)
	{
		const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
		RenderFrame(bb.Min, bb.Max, col, false, 0.0f);
	}*/

	static std::unordered_map<ImGuiID, SingleAnim> anim;
	auto map_itr = anim.find(id);

	auto pair = (map_itr != anim.end()) ? map_itr->second : SingleAnim{};
	auto alpha = pair.alpha;

	const float ANIM_SPEED = 1.0f; // bigger values mean faster change
	const float deltatime = g.IO.DeltaTime * ANIM_SPEED;

	float limitAlpha = hovered || selected ? ImColor(GetColorU32(ImGuiCol_Text)).Value.w : ImColor(GetColorU32(ImGuiCol_Text)).Value.w * 0.65f;

	if (alpha < limitAlpha)
		alpha = ImMin(alpha + deltatime, limitAlpha);
	else if (alpha > limitAlpha)
		alpha = ImMax(alpha - deltatime, limitAlpha);

	if (map_itr != anim.end())
		map_itr->second = SingleAnim{ alpha };
	else
		anim.emplace(std::make_pair(id, SingleAnim{ alpha }));

	RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);

	if (span_all_columns && window->DC.CurrentColumns)
		PopColumnsBackground();
	else if (span_all_columns && g.CurrentTable)
		TablePopBackgroundChannel();

	PushStyleColor(ImGuiCol_Text, GetColorU32(selected ? ImGuiCol_TextSelectable : ImGuiCol_Text, alpha));
	RenderTextClipped(text_min, text_max, label, NULL, &label_size, style.SelectableTextAlign, &bb);
	PopStyleColor();

	// Automatically close popups
	if (pressed && (window->Flags & ImGuiWindowFlags_Popup) && !(flags & ImGuiSelectableFlags_DontClosePopups) && !(g.LastItemData.InFlags & ImGuiItemFlags_SelectableDontClosePopup))
		CloseCurrentPopup();

	if (disabled_item && !disabled_global)
		EndDisabled();

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
	return pressed; //-V1020
}

static float CalcMaxPopupHeightFromItemCount(int items_count)
{
	ImGuiContext& g = *GImGui;
	if (items_count <= 0)
		return FLT_MAX;
	return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}

bool ImGui::Animated::Combo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items)
{
	ImGuiContext& g = *GImGui;

	// Call the getter to obtain the preview string which is a parameter to BeginCombo()
	const char* preview_value = NULL;
	if (*current_item >= 0 && *current_item < items_count)
		items_getter(data, *current_item, &preview_value);

	// The old Combo() API exposed "popup_max_height_in_items". The new more general BeginCombo() API doesn't have/need it, but we emulate it here.
	if (popup_max_height_in_items != -1 && !(g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint))
		SetNextWindowSizeConstraints(ImVec2(0, 0), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));

	if (!BeginCombo(label, preview_value, ImGuiComboFlags_None))
		return false;

	// Display items
	// FIXME-OPT: Use clipper (but we need to disable it on the appearing frame to make sure our call to SetItemDefaultFocus() is processed)
	bool value_changed = false;
	for (int i = 0; i < items_count; i++)
	{
		PushID(i);
		const bool item_selected = (i == *current_item);
		const char* item_text;
		if (!items_getter(data, i, &item_text))
			item_text = "*Unknown item*";
		if (Selectable(item_text, item_selected, 0, ImVec2(0, 0)))
		{
			value_changed = true;
			*current_item = i;
		}
		if (item_selected)
			SetItemDefaultFocus();
		PopID();
	}

	EndCombo();

	if (value_changed)
		MarkItemEdited(g.LastItemData.ID);

	return value_changed;
}

static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
	const char* const* items = (const char* const*)data;
	if (out_text)
		*out_text = items[idx];
	return true;
}

bool ImGui::Animated::Combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
{
	const bool value_changed = Combo(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
	return value_changed;
}

static const char* PatchFormatStringFloatToInt(const char* fmt)
{
	if (fmt[0] == '%' && fmt[1] == '.' && fmt[2] == '0' && fmt[3] == 'f' && fmt[4] == 0) // Fast legacy path for "%.0f" which is expected to be the most common case.
		return "%d";
	const char* fmt_start = ImParseFormatFindStart(fmt);    // Find % (if any, and ignore %%)
	const char* fmt_end = ImParseFormatFindEnd(fmt_start);  // Find end of format specifier, which itself is an exercise of confidence/recklessness (because snprintf is dependent on libc or user).
	if (fmt_end > fmt_start && fmt_end[-1] == 'f')
	{
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
		if (fmt_start == fmt && fmt_end[0] == 0)
			return "%d";
		ImGuiContext& g = *GImGui;
		ImFormatString(g.TempBuffer, IM_ARRAYSIZE(g.TempBuffer), "%.*s%%d%s", (int)(fmt_start - fmt), fmt, fmt_end); // Honor leading and trailing decorations, but lose alignment/precision.
		return g.TempBuffer;
#else
		IM_ASSERT(0 && "DragInt(): Invalid format string!"); // Old versions used a default parameter of "%.0f", please replace with e.g. "%d"
#endif
	}
	return fmt;
}

bool ImGui::Animated::SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const float w = CalcItemWidth();

	const ImVec2 label_size = CalcTextSize(label, NULL, true);
	const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y * 2.0f));
	const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

	const bool temp_input_allowed = (flags & ImGuiSliderFlags_NoInput) == 0;
	ItemSize(total_bb, style.FramePadding.y);
	if (!ItemAdd(total_bb, id, &frame_bb, temp_input_allowed ? ImGuiItemFlags_Inputable : 0))
		return false;

	// Default format string when passing NULL
	if (format == NULL)
		format = DataTypeGetInfo(data_type)->PrintFmt;
	else if (data_type == ImGuiDataType_S32 && strcmp(format, "%d") != 0) // (FIXME-LEGACY: Patch old "%.0f" format string to use "%d", read function more details.)
		format = PatchFormatStringFloatToInt(format);

	// Tabbing or CTRL-clicking on Slider turns it into an input box
	const bool hovered = ItemHoverable(frame_bb, id);
	bool temp_input_is_active = temp_input_allowed && TempInputIsActive(id);
	if (!temp_input_is_active)
	{
		const bool input_requested_by_tabbing = temp_input_allowed && (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_FocusedByTabbing) != 0;
		const bool clicked = (hovered && g.IO.MouseClicked[0]);
		if (input_requested_by_tabbing || clicked || g.NavActivateId == id || g.NavActivateInputId == id)
		{
			SetActiveID(id, window);
			SetFocusID(id, window);
			FocusWindow(window);
			g.ActiveIdUsingNavDirMask |= (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
			if (temp_input_allowed && (input_requested_by_tabbing || (clicked && g.IO.KeyCtrl) || g.NavActivateInputId == id))
				temp_input_is_active = true;
		}
	}

	if (temp_input_is_active)
	{
		// Only clamp CTRL+Click input when ImGuiSliderFlags_AlwaysClamp is set
		const bool is_clamp_input = (flags & ImGuiSliderFlags_AlwaysClamp) != 0;
		return TempInputScalar(frame_bb, id, label, data_type, p_data, format, is_clamp_input ? p_min : NULL, is_clamp_input ? p_max : NULL);
	}

	static std::unordered_map<ImGuiID, AlphaAnimOutline> anim;
	auto map_itr = anim.find(id);

	auto pair = (map_itr != anim.end()) ? map_itr->second : AlphaAnimOutline{};
	auto alpha = pair.alpha;
	auto outline = pair.alpha_outline;

	//const float limitAlpha = ImColor(GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg)).Value.w;
	const float limitAlpha = ImColor(GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg)).Value.w;
	const float limitAlphaOutline = hovered ? 1.0f : 0.4f; // breaking on lower alphas...

	const float ANIM_SPEED = 1.0f; // bigger values mean faster change
	const float ANIM_SPEED_CHECKBOX = 10.0f; // same as ^
	const float deltatime = g.IO.DeltaTime * ANIM_SPEED;

	if (alpha < limitAlpha)
		alpha = ImMin(alpha + deltatime, limitAlpha);
	else if (alpha > limitAlpha)
		alpha = ImMax(alpha - deltatime, limitAlpha);

	if (outline < limitAlphaOutline)
		outline = ImMin(outline + deltatime, limitAlphaOutline);
	else if (outline > limitAlphaOutline)
		outline = ImMax(outline - deltatime, limitAlphaOutline);

	if (map_itr != anim.end())
		map_itr->second = AlphaAnimOutline{ alpha, outline };
	else
		anim.emplace(std::make_pair(id, AlphaAnimOutline{ alpha, outline }));

	// Draw frame
	const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
	RenderNavHighlight(frame_bb, id);
	RenderFrameAlphaOutline(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding, alpha, outline);

	// Slider behavior
	ImRect grab_bb;
	const bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
	if (value_changed)
		MarkItemEdited(id);	

	// Render grab
	if (grab_bb.Max.x > grab_bb.Min.x)
		window->DrawList->AddCircleFilled(grab_bb.GetCenter(), (grab_bb.Max.y - grab_bb.Min.y) / 2.0f, GetColorU32(hovered ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab, outline), 32);

	// Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
	char value_buf[64];
	const char* value_buf_end = value_buf + DataTypeFormatString(value_buf, IM_ARRAYSIZE(value_buf), data_type, p_data, format);
	if (g.LogEnabled)
		LogSetNextTextDecoration("{", "}");
	RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

	if (label_size.x > 0.0f)
		RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
	return value_changed;
}

bool ImGui::Animated::SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
}

bool ImGui::Animated::SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalarN(label, ImGuiDataType_Float, v, 2, &v_min, &v_max, format, flags);
}

bool ImGui::Animated::SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalarN(label, ImGuiDataType_Float, v, 3, &v_min, &v_max, format, flags);
}

bool ImGui::Animated::SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalarN(label, ImGuiDataType_Float, v, 4, &v_min, &v_max, format, flags);
}

bool ImGui::Animated::SliderAngle(const char* label, float* v_rad, float v_degrees_min, float v_degrees_max, const char* format, ImGuiSliderFlags flags)
{
	if (format == NULL)
		format = "%.0f deg";
	float v_deg = (*v_rad) * 360.0f / (2 * IM_PI);
	bool value_changed = SliderFloat(label, &v_deg, v_degrees_min, v_degrees_max, format, flags);
	*v_rad = v_deg * (2 * IM_PI) / 360.0f;
	return value_changed;
}

bool ImGui::Animated::SliderInt(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
}

bool ImGui::Animated::SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalarN(label, ImGuiDataType_S32, v, 2, &v_min, &v_max, format, flags);
}

bool ImGui::Animated::SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalarN(label, ImGuiDataType_S32, v, 3, &v_min, &v_max, format, flags);
}

bool ImGui::Animated::SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalarN(label, ImGuiDataType_S32, v, 4, &v_min, &v_max, format, flags);
}

bool ImGui::Animated::ButtonEx(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	if ((flags & ImGuiButtonFlags_AlignTextBaseLine) && style.FramePadding.y < window->DC.CurrLineTextBaseOffset) // Try to vertically align buttons that are smaller/have no padding so that text baseline matches (bit hacky, since it shouldn't be a flag)
		pos.y += window->DC.CurrLineTextBaseOffset - style.FramePadding.y;
	ImVec2 size = CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ItemSize(size, style.FramePadding.y);
	if (!ItemAdd(bb, id))
		return false;

	if (g.LastItemData.InFlags & ImGuiItemFlags_ButtonRepeat)
		flags |= ImGuiButtonFlags_Repeat;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	static std::unordered_map<ImGuiID, ButtonAnim> anim;
	auto map_itr = anim.find(id);

	auto pair = (map_itr != anim.end()) ? map_itr->second : ButtonAnim{};
	auto alpha = pair.alpha;
	auto outline = pair.alpha_outline;
	auto thickness = pair.thickness;
	auto lastTime = pair.lastTime;

	const float ANIM_SPEED = 1.0f; // bigger values mean faster change
	const float ANIM_SPEED_CHECKBOX = 10.0f; // same as ^
	const float deltatime = g.IO.DeltaTime * ANIM_SPEED;

	// don't use global timers, they exist and may work well
	// however very slow animation while changing active ID will reset the timer, while animation didnt end
	// so we handle it on own in ID map
	if (held || pressed)
		lastTime = 0.0f;

	if (lastTime == 0.f && !pressed)
		lastTime = g.LastActiveIdTimer; // use global timer, this time it won't matter

	lastTime += g.IO.DeltaTime;

	const float temp_thickness = ImSaturate(lastTime * ANIM_SPEED_CHECKBOX); // magic number, looks nice
	thickness = held ? temp_thickness : (1.0f - temp_thickness);

	const ImColor frame_col = ImColor(GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button));
	const float limitAlpha = frame_col.Value.w;
	const float limitAlphaOutline = hovered ? 1.0f : 0.4f; // breaking on lower alphas...

	if (alpha < limitAlpha)
		alpha = ImMin(alpha + deltatime, limitAlpha);
	else if (alpha > limitAlpha)
		alpha = ImMax(alpha - deltatime, limitAlpha);

	if (outline < limitAlphaOutline)
		outline = ImMin(outline + deltatime, limitAlphaOutline);
	else if (outline > limitAlphaOutline)
		outline = ImMax(outline - deltatime, limitAlphaOutline);

	if (map_itr != anim.end())
		map_itr->second = ButtonAnim{ alpha, thickness, lastTime, outline };
	else
		anim.emplace(std::make_pair(id, ButtonAnim{ alpha, thickness, lastTime, outline }));

	const float size_frame_extra = size.x / 25.0f * thickness; // magic number, looks okay-ish
	const float half_size_extra = size_frame_extra / 2.0f;
	const float sizes_ratio = size.y / size.x;

	const ImRect frame_bb = ImRect(ImVec2(pos.x + half_size_extra, pos.y + (half_size_extra * sizes_ratio)),
		ImVec2(pos.x + (size.x - size_frame_extra), pos.y + (size.y - (size_frame_extra * sizes_ratio))));

	// Render
	const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	RenderNavHighlight(bb, id);
	RenderFrameAlphaOutline(frame_bb.Min, frame_bb.Max, col, true, style.FrameRounding, alpha, outline);

	if (g.LogEnabled)
		LogSetNextTextDecoration("[", "]");
	RenderTextClipped(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &frame_bb);

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
	return pressed;
}

bool ImGui::Animated::Button(const char* label, const ImVec2& size_arg)
{
	return ButtonEx(label, size_arg, ImGuiButtonFlags_None);
}

bool ImGui::Animated::ListBox(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int height_in_items)
{
	ImGuiContext& g = *GImGui;

	// Calculate size from "height_in_items"
	if (height_in_items < 0)
		height_in_items = ImMin(items_count, 7);
	float height_in_items_f = height_in_items + 0.25f;
	ImVec2 size(0.0f, ImFloor(GetTextLineHeightWithSpacing() * height_in_items_f + g.Style.FramePadding.y * 2.0f));

	if (!BeginListBox(label, size))
		return false;

	// Assume all items have even height (= 1 line of text). If you need items of different height,
	// you can create a custom version of ListBox() in your code without using the clipper.
	bool value_changed = false;
	ImGuiListClipper clipper;
	clipper.Begin(items_count, GetTextLineHeightWithSpacing()); // We know exactly our line height here so we pass it as a minor optimization, but generally you don't need to.
	while (clipper.Step())
		for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
		{
			const char* item_text;
			if (!items_getter(data, i, &item_text))
				item_text = "*Unknown item*";

			PushID(i);
			const bool item_selected = (i == *current_item);
			if (Selectable(item_text, item_selected))
			{
				*current_item = i;
				value_changed = true;
			}
			if (item_selected)
				SetItemDefaultFocus();
			PopID();
		}
	EndListBox();

	if (value_changed)
		MarkItemEdited(g.LastItemData.ID);

	return value_changed;
}