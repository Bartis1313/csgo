#include "styleEditor.hpp"

#include "../common.hpp"
#include "../background.hpp"

#include <imgui_freetype.h>
#include <imgui_internal.h>

void styleEditor::draw()
{
	// You can pass in a reference ImGuiStyle structure to compare to, revert to and save to
	// (without a reference style pointer, we will use one compared locally as a reference)
	ImGuiStyle& style = ImGui::GetStyle();
	static ImGuiStyle ref_saved_style = style;
	ImGuiStyle* ref = &ref_saved_style;
	// ugly methods for custom colors, TODO: do own struct for this
	static auto ref_saved_gradient = vars::styling->groupPanelBackground;
	static auto ref_saved_color_panel_line = vars::styling->groupPanelLines;

	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);

	ImGui::Checkbox("Background", &vars::styling->background);
	ImGui::SliderFloat("Background speed", &vars::styling->speed, 1.0f, 8.0f);
	ImGui::SliderFloat("Background max dist", &vars::styling->distance, 10.0f, 400.0f);
	ImGui::SliderInt("Background amount of records", &vars::styling->size, 20, 400);
	ImGui::Text("They are temp limited with dynamic size + saving!");
	ImGui::ColorPicker("Background color1", &vars::styling->color1);
	ImGui::SameLine();
	ImGui::ColorPicker("Background color2", &vars::styling->color2);
	ImGui::SameLine();
	ImGui::ColorPicker("Background color3", &vars::styling->color3);
	if (ImGui::Button("Refresh background"))
		background::init();

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
	ImGui::HelpMarker(
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
			ImGui::SameLine(); ImGui::HelpMarker("Alignment applies when a button is larger than its text content.");
			changedbut |= ImGui::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
			ImGui::SameLine(); ImGui::HelpMarker("Alignment applies when a selectable is larger than its text content.");
			ImGui::Text("Safe Area Padding");
			ImGui::SameLine(); ImGui::HelpMarker("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
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
			if (ImGui::Button("Export"))
			{
				if (output_dest == 0)
					ImGui::LogToClipboard();
				else
					ImGui::LogToTTY();
				ImGui::LogText("ImVec4* colors = ImGui::GetStyle().Colors;" IM_NEWLINE);
				for (int i = 0; i < ImGuiCol_COUNT; i++)
				{
					const ImVec4& col = style.Colors[i];
					const char* name = ImGui::GetStyleColorName(i);
					if (!output_only_modified || memcmp(&col, &ref->Colors[i], sizeof(ImVec4)) != 0)
						ImGui::LogText("colors[ImGuiCol_%s]%*s= ImVec4(%.2ff, %.2ff, %.2ff, %.2ff);" IM_NEWLINE,
							name, 23 - (int)strlen(name), "", col.x, col.y, col.z, col.w);
				}
				ImGui::LogFinish();
			}
			ImGui::SameLine(); ImGui::SetNextItemWidth(120); ImGui::Combo("##output_type", &output_dest, "To Clipboard\0To TTY\0");
			ImGui::SameLine(); ImGui::Checkbox("Only Modified Colors", &output_only_modified);

			static ImGuiTextFilter filter;
			filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

			static ImGuiColorEditFlags alpha_flags = 0;
			if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None)) { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
			if (ImGui::RadioButton("Alpha", alpha_flags == ImGuiColorEditFlags_AlphaPreview)) { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
			if (ImGui::RadioButton("Both", alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();
			ImGui::HelpMarker(
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
			ImGui::TextWrapped(
				"Below we are displaying the font texture (which is the only texture we have access to in this demo). "
				"Use the 'ImTextureID' type as storage to pass pointers or identifier to your own texture data. "
				"Hover the texture for a zoomed view!");

			// Below we are displaying the font texture because it is the only texture we have access to inside the demo!
			// Remember that ImTextureID is just storage for whatever you want it to be. It is essentially a value that
			// will be passed to the rendering backend via the ImDrawCmd structure.
			// If you use one of the default imgui_impl_XXXX.cpp rendering backend, they all have comments at the top
			// of their respective source file to specify what they expect to be stored in ImTextureID, for example:
			// - The imgui_impl_dx11.cpp renderer expect a 'ID3D11ShaderResourceView*' pointer
			// - The imgui_impl_opengl3.cpp renderer expect a GLuint OpenGL texture identifier, etc.
			// More:
			// - If you decided that ImTextureID = MyEngineTexture*, then you can pass your MyEngineTexture* pointers
			//   to ImGui::Image(), and gather width/height through your own functions, etc.
			// - You can use ShowMetricsWindow() to inspect the draw data that are being passed to your renderer,
			//   it will help you debug issues if you are confused about it.
			// - Consider using the lower-level ImDrawList::AddImage() API, via ImGui::GetWindowDrawList()->AddImage().
			// - Read https://github.com/ocornut/imgui/blob/master/docs/FAQ.md
			// - Read https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
			ImTextureID my_tex_id = io.Fonts->TexID;
			float my_tex_w = (float)io.Fonts->TexWidth;
			float my_tex_h = (float)io.Fonts->TexHeight;
			{
				static bool use_text_color_for_tint = false;
				ImGui::Checkbox("Use Text Color for Tint", &use_text_color_for_tint);
				ImGui::Text("%.0fx%.0f", my_tex_w, my_tex_h);
				ImVec2 pos = ImGui::GetCursorScreenPos();
				ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
				ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
				ImVec4 tint_col = use_text_color_for_tint ? ImGui::GetStyleColorVec4(ImGuiCol_Text) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
				ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
				ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h), uv_min, uv_max, tint_col, border_col);
				if (ImGui::IsItemHovered() && ImGui::BeginTooltip())
				{
					float region_sz = 32.0f;
					float region_x = io.MousePos.x - pos.x - region_sz * 0.5f;
					float region_y = io.MousePos.y - pos.y - region_sz * 0.5f;
					float zoom = 4.0f;
					if (region_x < 0.0f) { region_x = 0.0f; }
					else if (region_x > my_tex_w - region_sz) { region_x = my_tex_w - region_sz; }
					if (region_y < 0.0f) { region_y = 0.0f; }
					else if (region_y > my_tex_h - region_sz) { region_y = my_tex_h - region_sz; }
					ImGui::Text("Min: (%.2f, %.2f)", region_x, region_y);
					ImGui::Text("Max: (%.2f, %.2f)", region_x + region_sz, region_y + region_sz);
					ImVec2 uv0 = ImVec2((region_x) / my_tex_w, (region_y) / my_tex_h);
					ImVec2 uv1 = ImVec2((region_x + region_sz) / my_tex_w, (region_y + region_sz) / my_tex_h);
					ImGui::Image(my_tex_id, ImVec2(region_sz * zoom, region_sz * zoom), uv0, uv1, tint_col, border_col);
					ImGui::EndTooltip();
				}
			}

			// Post-baking font scaling. Note that this is NOT the nice way of scaling fonts, read below.
			// (we enforce hard clamping manually as by default DragFloat/SliderFloat allows CTRL+Click text to get out of bounds).
			const float MIN_SCALE = 0.3f;
			const float MAX_SCALE = 2.0f;
			ImGui::HelpMarker(
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
			ImGui::HelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");

			ImGui::Checkbox("Anti-aliased lines use texture", &style.AntiAliasedLinesUseTex);
			ImGui::SameLine();
			ImGui::HelpMarker("Faster lines using texture data. Require backend to render with bilinear filtering (not point/nearest filtering).");

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
			ImGui::HelpMarker("When drawing circle primitives with \"num_segments == 0\" tesselation will be calculated automatically.");

			ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
			ImGui::DragFloat("Disabled Alpha", &style.DisabledAlpha, 0.005f, 0.0f, 1.0f, "%.2f"); ImGui::SameLine(); ImGui::HelpMarker("Additional alpha multiplier for disabled items (multiply over current value of Alpha).");
			ImGui::PopItemWidth();

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::PopItemWidth();
}