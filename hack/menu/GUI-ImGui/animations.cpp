#include "animations.hpp"

#include "menu.hpp"

#include <config/config.hpp>
#include <utilities/utilities.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>

void ImGui::Animations::setAnimationSpeeds()
{
	detail::speedAlpha = vars::styling->smoothAlpha;
	detail::speedOutline = vars::styling->smoothOutline;
	detail::speedMove = vars::styling->speedMove;
	detail::speedPop = vars::styling->smoothPop;
}

ImGui::Animations::AlphaAnimation ImGui::Animations::getAlpha(ImGuiID id, const AlphaLimits& limits)
{
	ImGuiContext& g = *GImGui;

	AlphaAnimation anim;
	const auto itr = alphas.find(id);
	if (itr != alphas.end())
		anim = itr->second;

	anim.alpha = ImLerpSmooth(anim.alpha, limits.alphaLimit, g.IO.DeltaTime, detail::speedAlpha);
	anim.outline = ImLerpSmooth(anim.outline, limits.outlineLimit, g.IO.DeltaTime, detail::speedOutline);

	alphas[id] = anim;
	return anim;
}

ImGui::Animations::SingleAnimation ImGui::Animations::getSingleAnimation(ImGuiID id, float max, float duration, float extraSpped)
{
	ImGuiContext& g = *GImGui;

	SingleAnimation anim;
	const auto itr = singleAnim.find(id);
	if (itr != singleAnim.end())
		anim = itr->second;

	anim.value = ImLerpSmooth(anim.value, max, g.IO.DeltaTime * extraSpped, duration);

	singleAnim[id] = anim;
	return anim;
}

ImGui::Animations::MoveAnimation ImGui::Animations::getMove(ImGuiID id, float lastTime, bool held, bool pressed)
{
	MoveAnimation move;
	const auto itr = moves.find(id);
	if (itr != moves.end())
		move = itr->second;

	const float temp_thickness = ImSaturate(lastTime * detail::speedMove);
	move.move = held ? temp_thickness : (1.0f - temp_thickness);

	moves[id] = move;
	return move;
}

ImGui::Animations::SingleField ImGui::Animations::getAndUpdateSingleField(ImGuiID id, float value)
{
	SingleField field = { .value = value };
	singleFields[id] = SingleField{ .value = value };
	return field;
}

float ImGui::Animations::getLastTime(ImGuiID id, bool held, bool pressed)
{
	ImGuiContext& g = *GImGui;

	float lastTime = 0.0f;
	const auto itr = timeStack.find(id);
	if (itr != timeStack.end())
		lastTime = itr->second;

	if (held || pressed)
		lastTime = 0.0f;

	if (lastTime == 0.f && !pressed)
		lastTime = g.LastActiveIdTimer;

	lastTime += g.IO.DeltaTime;
	timeStack[id] = lastTime;

	return lastTime;
}

void ImGui::Animations::RenderArrow(ImDrawList* draw_list, ImVec2 pos, ImU32 col, ImGuiDir dir, float angle, float scale)
{
	const float h = draw_list->_Data->FontSize * 1.00f;
	float r = h * 0.40f * scale;
	ImVec2 center = pos + ImVec2(h * 0.50f, h * 0.50f * scale);
	ImVec2 a, b, c;

	const float cangle = ImCos(angle);
	const float sangle = ImSin(angle);

	switch (dir)
	{
	case ImGuiDir_Up:
	case ImGuiDir_Down:
		if (dir == ImGuiDir_Up) r = -r;
		a = ImRotate(ImVec2(+0.000f, +0.750f), cangle, sangle) * r;
		b = ImRotate(ImVec2(-0.866f, -0.750f), cangle, sangle) * r;
		c = ImRotate(ImVec2(+0.866f, -0.750f), cangle, sangle) * r;
		break;
	case ImGuiDir_Left:
	case ImGuiDir_Right:
		if (dir == ImGuiDir_Left) r = -r;
		a = ImRotate(ImVec2(+0.750f, +0.000f), cangle, sangle) * r;
		b = ImRotate(ImVec2(-0.750f, +0.866f), cangle, sangle) * r;
		c = ImRotate(ImVec2(-0.750f, -0.866f), cangle, sangle) * r;
		break;
	case ImGuiDir_None:
	case ImGuiDir_COUNT:
		IM_ASSERT(0);
		break;
	}

	draw_list->AddTriangleFilled(center + a, center + b, center + c, col);
}

static float CalcMaxPopupHeightFromItemCount(int items_count)
{
	ImGuiContext& g = *GImGui;
	if (items_count <= 0)
		return FLT_MAX;
	return (g.FontSize + g.Style.ItemSpacing.y) * items_count - g.Style.ItemSpacing.y + (g.Style.WindowPadding.y * 2);
}

bool ImGui::Animations::BeginComboPopup(ImGuiID popup_id, const ImRect& bb, ImGuiComboFlags flags, float pop)
{
	ImGuiContext& g = *GImGui;
	if (!IsPopupOpen(popup_id, ImGuiPopupFlags_None))
	{
		g.NextWindowData.ClearFlags();
		return false;
	}

	// Set popup size
	float w = bb.GetWidth() * pop;
	if (g.NextWindowData.Flags & ImGuiNextWindowDataFlags_HasSizeConstraint)
	{
		g.NextWindowData.SizeConstraintRect.Min.x = ImMax(g.NextWindowData.SizeConstraintRect.Min.x, w);
	}
	else
	{
		if ((flags & ImGuiComboFlags_HeightMask_) == 0)
			flags |= ImGuiComboFlags_HeightRegular;
		IM_ASSERT(ImIsPowerOfTwo(flags & ImGuiComboFlags_HeightMask_)); // Only one
		int popup_max_height_in_items = -1;
		if (flags & ImGuiComboFlags_HeightRegular)     popup_max_height_in_items = 8;
		else if (flags & ImGuiComboFlags_HeightSmall)  popup_max_height_in_items = 4;
		else if (flags & ImGuiComboFlags_HeightLarge)  popup_max_height_in_items = 20;
		SetNextWindowSizeConstraints(ImVec2(w, 0.0f), ImVec2(FLT_MAX, CalcMaxPopupHeightFromItemCount(popup_max_height_in_items)));
	}

	// This is essentially a specialized version of BeginPopupEx()
	char name[16];
	ImFormatString(name, IM_ARRAYSIZE(name), "##Combo_%02d", g.BeginPopupStack.Size); // Recycle windows based on depth

	// Set position given a custom constraint (peak into expected window size so we can position it)
	// FIXME: This might be easier to express with an hypothetical SetNextWindowPosConstraints() function?
	// FIXME: This might be moved to Begin() or at least around the same spot where Tooltips and other Popups are calling FindBestWindowPosForPopupEx()?
	if (ImGuiWindow* popup_window = FindWindowByName(name))
		if (popup_window->WasActive)
		{
			// Always override 'AutoPosLastDirection' to not leave a chance for a past value to affect us.
			ImVec2 size_expected = CalcWindowNextAutoFitSize(popup_window);
			popup_window->AutoPosLastDirection = (flags & ImGuiComboFlags_PopupAlignLeft) ? ImGuiDir_Left : ImGuiDir_Down; // Left = "Below, Toward Left", Down = "Below, Toward Right (default)"
			ImRect r_outer = GetPopupAllowedExtentRect(popup_window);
			ImVec2 pos = FindBestWindowPosForPopupEx(bb.GetBL(), size_expected, &popup_window->AutoPosLastDirection, r_outer, bb, ImGuiPopupPositionPolicy_ComboBox);
			SetNextWindowPos(pos);
		}

	// We don't use BeginPopupEx() solely because we have a custom name string, which we could make an argument to BeginPopupEx()
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove;
	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(g.Style.FramePadding.x, g.Style.WindowPadding.y)); // Horizontally align ourselves with the framed text
	bool ret = Begin(name, NULL, window_flags);
	PopStyleVar();
	if (!ret)
	{
		EndPopup();
		IM_ASSERT(0);   // This should never happen as we tested for IsPopupOpen() above
		return false;
	}
	return true;
}

void ImGui::Animations::RenderCheckMarkExtra(ImDrawList* draw_list, ImVec2 pos, ImU32 col, float sz, float thickness_extra)
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

void ImGui::Animations::RenderFrameAlpha(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding, float alpha)
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

void ImGui::Animations::RenderFrameAlphaOutline(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding, float alpha, float outline)
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

void ImGui::Animations::RenderFrameOutlineKeepFill(ImVec2 p_min, ImVec2 p_max, ImU32 fill_col, bool border, float rounding, float outline)
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

void ImGui::Animations::RenderTextAlpha(ImVec2 pos, const char* text, const char* text_end, bool hide_text_after_hash, float alpha)
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

void ImGui::Animations::RenderFrameBorderAlpha(ImVec2 p_min, ImVec2 p_max, float rounding, float alpha)
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

// because some update made stack for those items kinda different, I will use own non api-like solution to solve this
static std::unordered_map<ImGuiID, bool> stackHotkey;

bool ImGui::Animations::Hotkey(const char* label, Key* key, bool useExtended, const ImVec2& size)
{
	ImGui::PushID(label);
	if (std::strncmp(label, "##", 2))
		ImGui::TextUnformatted(label, std::strstr(label, "##"));

	ImGui::SameLine();
	const auto id = ImGui::GetID(label);

	bool isInHotkey = stackHotkey[id];

	if (isInHotkey)
	{
		ImGui::Animations::Button("...", size);
		ImGui::SetItemAllowOverlap();

		if (key->checkKey())
		{
			globals::isInHotkey = false;
			stackHotkey[id] = false;
			ImGui::ClearActiveID();
		}

	}
	else if (ImGui::Animations::Button(utilities::getKeyName(key->getKeyCode()).c_str(), size))
	{
		globals::isInHotkey = true;
		stackHotkey[id] = true;
		ImGui::SetActiveID(id, GetCurrentWindow());
	}

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
		if (IsItemHovered())
		{
			ImGui::SetTooltip("Key mode");

			if (ImGui::GetIO().MouseClicked[1])
				ImGui::OpenPopup("##pop");
		}
	}

	ImGui::PopID();

	return true;
}

bool ImGui::Animations::Checkbox(const char* label, bool* v)
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

	const auto lastTime = getLastTime(id, held, pressed);
	const auto move = getMove(id, lastTime, held, pressed);
	const float temp = ImSaturate(lastTime * detail::speedMove);
	float checkmark = checkmark = *v ? temp : 0.0f;

	const ImColor frame_col = ImColor(GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg));
	const float limitAlpha = frame_col.Value.w;
	const float limitAlphaOutline = hovered ? 1.0f : 0.4f; // breaking on lower alphas...

	const auto alphas = getAlpha(id, AlphaLimits{ .alphaLimit = limitAlpha, .outlineLimit = limitAlphaOutline });

	const float size_frame_extra = 3.f * move.move;
	ImVec2 extra_size = ImVec2(size_frame_extra, size_frame_extra);

	const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
	const ImRect frame_bb(ImVec2(pos + extra_size), pos + ImVec2(square_sz, square_sz) - extra_size);
	RenderNavHighlight(total_bb, id);
	RenderFrameAlphaOutline(frame_bb.Min, frame_bb.Max, frame_col, true, style.FrameRounding, alphas.alpha, alphas.outline);

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

bool ImGui::Animations::BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags)
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

	const float limitAlpha = ImColor(GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg)).Value.w;
	const float limitAlphaOutline = hovered || popup_open ? 1.0f : 0.4f; // breaking on lower alphas...

	const auto alphas = getAlpha(id, AlphaLimits{ .alphaLimit = limitAlpha, .outlineLimit = limitAlphaOutline });
	const float limitPop = popup_open ? 1.0f : 0.0f;
	const auto pop = getSingleAnimation(id, limitPop, detail::speedPop, 6.0f); // lerping with normal time will be slow

	// Render shape
	const ImColor frame_col = ImColor(GetColorU32(hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg));
	const float value_x2 = ImMax(bb.Min.x, bb.Max.x - arrow_size);
	RenderNavHighlight(bb, id);
	if (!(flags & ImGuiComboFlags_NoPreview))
		window->DrawList->AddRectFilled(bb.Min, ImVec2(ImMax(bb.Min.x, bb.Max.x - 1.0f), bb.Max.y), frame_col, style.FrameRounding);
	if (!(flags & ImGuiComboFlags_NoArrowButton))
	{
		//ImU32 bg_col = GetColorU32((popup_open || hovered) ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
		ImU32 text_col = GetColorU32(ImGuiCol_Text, alphas.alpha);
		//window->DrawList->AddRectFilled(ImVec2(value_x2, bb.Min.y), bb.Max, bg_col, style.FrameRounding, (w <= arrow_size) ? ImDrawFlags_RoundCornersAll : ImDrawFlags_RoundCornersRight);
		if (value_x2 + arrow_size - style.FramePadding.x <= bb.Max.x)
			RenderArrow(window->DrawList, ImVec2(value_x2 + style.FramePadding.y, bb.Min.y + style.FramePadding.y), text_col, ImGuiDir_Down, -IM_PI * pop.value, 1.0f);
	}
	RenderFrameBorderAlpha(bb.Min, bb.Max, style.FrameRounding, alphas.outline);

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
	return BeginComboPopup(popup_id, bb, flags, pop.value);
}

bool ImGui::Animations::ColorPicker(const char* label, CfgColor* clr)
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

	bool openPopup = ImGui::Animations::ColorButton("##colbut", ImVec4{ clr->getColor().r(), clr->getColor().g(), clr->getColor().b(), clr->getColor().a() }, pickerButtonFlags);

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

				if (ImGui::Animations::ColorButton("##palette", ImVec4{ el.r(), el.g(), el.b(), el.a() }, paletteButoonFlags, paletteButtonSize))
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

bool ImGui::Animations::PopupButton(const char* label, const std::function<void()>& fun)
{
	ImGui::PushID(label);

	if (ImGui::Animations::Button("Options"))
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

bool ImGui::Animations::Selectable(const char* label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size_arg)
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
	if((flags & ImGuiSelectableFlags_SelectOnNav) && g.NavJustMovedToId != 0 && g.NavJustMovedToFocusScopeId == g.CurrentFocusScopeId)
		if (g.NavJustMovedToId == id)
			selected = pressed = true;

	// Update NavId when clicking or when Hovering (this doesn't happen on most widgets), so navigation can be resumed with gamepad/keyboard
	if (pressed || (hovered && (flags & ImGuiSelectableFlags_SetNavIdOnHover)))
	{
		if (!g.NavDisableMouseHover && g.NavWindow == window && g.NavLayer == window->DC.NavLayerCurrent)
		{
			SetNavID(id, window->DC.NavLayerCurrent, g.CurrentFocusScopeId, WindowRectAbsToRel(window, bb)); // (bb == NavRect)
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
	/*if (hovered || selected)
	{
		const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
		RenderFrame(bb.Min, bb.Max, col, false, 0.0f);
	}*/

	float limitAlpha = hovered || selected ? ImColor(GetColorU32(ImGuiCol_Text)).Value.w : ImColor(GetColorU32(ImGuiCol_Text)).Value.w * 0.65f;

	const auto alpha = getSingleAnimation(id, limitAlpha, detail::speedAlpha);

	RenderNavHighlight(bb, id, ImGuiNavHighlightFlags_TypeThin | ImGuiNavHighlightFlags_NoRounding);

	if (span_all_columns && window->DC.CurrentColumns)
		PopColumnsBackground();
	else if (span_all_columns && g.CurrentTable)
		TablePopBackgroundChannel();

	const auto ImGuiCol_TextSelectable = ImVec4{ 0.26f, 0.59f, 0.98f, alpha.value };
	const auto colText = ImGui::ColorConvertU32ToFloat4(GetColorU32(ImGuiCol_Text, alpha.value));

	PushStyleColor(ImGuiCol_Text, GetColorU32(selected ? ImGuiCol_TextSelectable : colText));
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

bool ImGui::Animations::Combo(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int popup_max_height_in_items)
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

bool ImGui::Animations::Combo(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
{
	const bool value_changed = Combo(label, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
	return value_changed;
}

bool ImGui::Animations::SliderScalar(const char* label, ImGuiDataType data_type, void* p_data, const void* p_min, const void* p_max, const char* format, ImGuiSliderFlags flags)
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

	//const float limitAlpha = ImColor(GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg)).Value.w;
	const float limitAlpha = ImColor(GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg)).Value.w;
	const float limitAlphaOutline = hovered ? 1.0f : 0.4f; // breaking on lower alphas...

	const auto alpha = getAlpha(id, AlphaLimits{ .alphaLimit = limitAlpha, .outlineLimit = limitAlphaOutline });

	// Draw frame
	const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
	RenderNavHighlight(frame_bb, id);
	RenderFrameAlphaOutline(frame_bb.Min, frame_bb.Max, frame_col, true, g.Style.FrameRounding, alpha.alpha, alpha.outline);

	// Slider behavior
	ImRect grab_bb;
	const bool value_changed = SliderBehavior(frame_bb, id, data_type, p_data, p_min, p_max, format, flags, &grab_bb);
	if (value_changed)
		MarkItemEdited(id);

	// Render grab
	if (grab_bb.Max.x > grab_bb.Min.x)
		window->DrawList->AddRectFilled(grab_bb.Min, grab_bb.Max, GetColorU32(hovered ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab, alpha.outline), style.GrabRounding);
	/*window->DrawList->AddCircleFilled(grab_bb.GetCenter(), (grab_bb.Max.y - grab_bb.Min.y) / 2.0f, GetColorU32(hovered ? ImGuiCol_SliderGrabActive : ImGuiCol_SliderGrab, outline), 32);*/

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

bool ImGui::Animations::SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalar(label, ImGuiDataType_Float, v, &v_min, &v_max, format, flags);
}

bool ImGui::Animations::SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalarN(label, ImGuiDataType_Float, v, 2, &v_min, &v_max, format, flags);
}

bool ImGui::Animations::SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalarN(label, ImGuiDataType_Float, v, 3, &v_min, &v_max, format, flags);
}

bool ImGui::Animations::SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalarN(label, ImGuiDataType_Float, v, 4, &v_min, &v_max, format, flags);
}

bool ImGui::Animations::SliderAngle(const char* label, float* v_rad, float v_degrees_min, float v_degrees_max, const char* format, ImGuiSliderFlags flags)
{
	if (format == NULL)
		format = "%.0f deg";
	float v_deg = (*v_rad) * 360.0f / (2 * IM_PI);
	bool value_changed = SliderFloat(label, &v_deg, v_degrees_min, v_degrees_max, format, flags);
	*v_rad = v_deg * (2 * IM_PI) / 360.0f;
	return value_changed;
}

bool ImGui::Animations::SliderInt(const char* label, int* v, int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalar(label, ImGuiDataType_S32, v, &v_min, &v_max, format, flags);
}

bool ImGui::Animations::SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalarN(label, ImGuiDataType_S32, v, 2, &v_min, &v_max, format, flags);
}

bool ImGui::Animations::SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalarN(label, ImGuiDataType_S32, v, 3, &v_min, &v_max, format, flags);
}

bool ImGui::Animations::SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* format, ImGuiSliderFlags flags)
{
	return SliderScalarN(label, ImGuiDataType_S32, v, 4, &v_min, &v_max, format, flags);
}

bool ImGui::Animations::ButtonEx(const char* label, const ImVec2& size_arg, ImGuiButtonFlags flags)
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

	const auto lastTime = getLastTime(id, held, pressed);
	const auto move = getMove(id, lastTime, held, pressed);

	const ImColor frame_col = ImColor(GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button));
	const float limitAlpha = frame_col.Value.w;
	const float limitAlphaOutline = hovered ? 1.0f : 0.4f; // breaking on lower alphas...

	const auto alphas = getAlpha(id, AlphaLimits{ .alphaLimit = limitAlpha, .outlineLimit = limitAlphaOutline });

	const float size_frame_extra = 3.f * move.move;
	ImVec2 extra_size = ImVec2(size_frame_extra, size_frame_extra);

	const ImRect frame_bb(ImVec2(pos + extra_size), pos + size - extra_size);

	// Render
	const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
	RenderNavHighlight(bb, id);
	RenderFrameAlphaOutline(frame_bb.Min, frame_bb.Max, col, true, style.FrameRounding, alphas.alpha, alphas.outline);

	if (g.LogEnabled)
		LogSetNextTextDecoration("[", "]");
	const ImVec2 pos_min = frame_bb.Min + style.FramePadding - (frame_bb.Min - bb.Min);
	const ImVec2 pos_max = frame_bb.Max - style.FramePadding - (frame_bb.Max - bb.Max);
	// I tried changing font size dynamically just for this text to use raw frame_bb.Min + style.FramePadding
	// for good effect it requires font reset, this thing wont be very friendly for cpu
	RenderTextClipped(pos_min, pos_max, label, NULL, &label_size, style.ButtonTextAlign, &frame_bb);

	IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
	return pressed;
}

bool ImGui::Animations::Button(const char* label, const ImVec2& size_arg)
{
	return ButtonEx(label, size_arg, ImGuiButtonFlags_None);
}

bool ImGui::Animations::ListBox(const char* label, int* current_item, bool (*items_getter)(void*, int, const char**), void* data, int items_count, int height_in_items)
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

bool ImGui::Animations::ColorButton(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags, ImVec2 size)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiID id = window->GetID(desc_id);
	float default_size = GetFrameHeight();
	if (size.x == 0.0f)
		size.x = default_size;
	if (size.y == 0.0f)
		size.y = default_size;
	const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
	ItemSize(bb, (size.y >= default_size) ? g.Style.FramePadding.y : 0.0f);
	if (!ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ButtonBehavior(bb, id, &hovered, &held);

	if (flags & ImGuiColorEditFlags_NoAlpha)
		flags &= ~(ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf);

	ImVec4 col_rgb = col;
	if (flags & ImGuiColorEditFlags_InputHSV)
		ColorConvertHSVtoRGB(col_rgb.x, col_rgb.y, col_rgb.z, col_rgb.x, col_rgb.y, col_rgb.z);

	ImVec4 col_rgb_without_alpha(col_rgb.x, col_rgb.y, col_rgb.z, 1.0f);
	float grid_step = ImMin(size.x, size.y) / 2.99f;
	float rounding = ImMin(g.Style.FrameRounding, grid_step * 0.5f);
	ImRect bb_inner = bb;
	float off = 0.0f;

	const auto lastTime = getLastTime(id, held, pressed);
	const auto move = getMove(id, lastTime, held, pressed);

	const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
	const float limitAlpha = ImColor(frame_col).Value.w;
	const float limitAlphaOutline = hovered ? 1.0f : 0.4f; // breaking on lower alphas...

	const auto alphas = getAlpha(id, AlphaLimits{ .alphaLimit = limitAlpha, .outlineLimit = limitAlphaOutline });

	const float size_frame_extra = 3.f * move.move;
	ImVec2 extra_size = ImVec2(size_frame_extra, size_frame_extra);

	if ((flags & ImGuiColorEditFlags_NoBorder) == 0)
	{
		off = -0.75f; // The border (using Col_FrameBg) tends to look off when color is near-opaque and rounding is enabled. This offset seemed like a good middle ground to reduce those artifacts.
		bb_inner.Expand(off);
	}
	if ((flags & ImGuiColorEditFlags_AlphaPreviewHalf) && col_rgb.w < 1.0f)
	{
		float mid_x = IM_ROUND((bb_inner.Min.x + bb_inner.Max.x) * 0.5f);
		RenderColorRectWithAlphaCheckerboard(window->DrawList, ImVec2(bb_inner.Min.x + grid_step, bb_inner.Min.y) + extra_size, bb_inner.Max - extra_size, GetColorU32(col_rgb), grid_step, ImVec2(-grid_step + off, off), rounding, ImDrawFlags_RoundCornersRight);
		window->DrawList->AddRectFilled(ImVec2(bb_inner.Min.x + grid_step, bb_inner.Min.y) + extra_size, bb_inner.Max - extra_size, GetColorU32(col_rgb), rounding, ImDrawFlags_RoundCornersLeft);
	}
	else
	{
		// Because GetColorU32() multiplies by the global style Alpha and we don't want to display a checkerboard if the source code had no alpha
		ImVec4 col_source = (flags & ImGuiColorEditFlags_AlphaPreview) ? col_rgb : col_rgb_without_alpha;
		if (col_source.w < 1.0f)
			RenderColorRectWithAlphaCheckerboard(window->DrawList, bb_inner.Min + extra_size, bb_inner.Max - extra_size, GetColorU32(col_source), grid_step, ImVec2(off, off), rounding);
		else
			window->DrawList->AddRectFilled(bb_inner.Min + extra_size, bb_inner.Max - extra_size, GetColorU32(col_source), rounding);
	}
	RenderNavHighlight(bb, id);
	if ((flags & ImGuiColorEditFlags_NoBorder) == 0)
	{
		if (g.Style.FrameBorderSize > 0.0f)
			RenderFrameBorderAlpha(bb.Min + extra_size, bb.Max - extra_size, rounding, alphas.outline);
		else
			window->DrawList->AddRect(bb.Min + extra_size, bb.Max - extra_size, GetColorU32(ImGuiCol_FrameBg, alphas.outline), rounding); // Color button are often in need of some sort of border
	}

	// Drag and Drop Source
	// NB: The ActiveId test is merely an optional micro-optimization, BeginDragDropSource() does the same test.
	if (g.ActiveId == id && !(flags & ImGuiColorEditFlags_NoDragDrop) && BeginDragDropSource())
	{
		if (flags & ImGuiColorEditFlags_NoAlpha)
			SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F, &col_rgb, sizeof(float) * 3, ImGuiCond_Once);
		else
			SetDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F, &col_rgb, sizeof(float) * 4, ImGuiCond_Once);
		ColorButton(desc_id, col, flags);
		SameLine();
		TextEx("Color");
		EndDragDropSource();
	}

	// Tooltip
	if (!(flags & ImGuiColorEditFlags_NoTooltip) && hovered)
		ColorTooltip(desc_id, &col.x, flags & (ImGuiColorEditFlags_InputMask_ | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaPreviewHalf));

	return pressed;
}

#include "imguiaddons.hpp"

void ImGui::Animations::Window(const char* label, bool* opened, const ImGui::Animations::WindowConfig& config, const std::function<void()>& callback)
{
	const ImGuiID id = ImGui::GetID(label);
	ImGuiIO& io = ImGui::GetIO();

	auto itr = ImGui::extraGlobals::settings.find(id);
	const bool newWindow = itr == ImGui::extraGlobals::settings.end();
	if (newWindow)
	{
		ImGui::extraGlobals::settings[id] = ImWindowSettings
		{
			.id = id,
			.pos = config.defaultPos + (config.defaultSize / 2),
			.size = ImVec2{ 0, 0 },
			.targetSize = config.defaultSize,
			.alpha = 0.0f
		};
		// force itr to be updated
		itr = ImGui::extraGlobals::settings.find(id);
	}

	auto inTransmission = [opened, config, itr](const auto min, const auto max)
	{
		constexpr float lerpThreshold = 0.01f;

		if (itr->second.alpha >= max - lerpThreshold)
			return false;

		if (itr->second.alpha <= min + lerpThreshold)
			return false;

		return true;
	};

	auto isMovedWindow = [id]()
	{
		const ImGuiWindow* movingWindow = GImGui->MovingWindow;
		if (movingWindow == nullptr)
			return false;

		/*if (movingWindow->ID != id)
			return false;*/

		return true;
	};

	auto isResizedWindow = []()
	{
		for (int i = 0; i < 4; i++)
		{
			if (ImGui::GetWindowResizeCornerID(ImGui::GetCurrentWindow(), i) != ImGui::GetActiveID())
				continue;

			return true;
		}

		for (int i = 0; i < 4; i++)
		{
			if (ImGui::GetWindowResizeBorderID(ImGui::GetCurrentWindow(), i) != ImGui::GetActiveID())
				continue;

			return true;
		}

		return false;
	};

	
	auto renderGui = [&](ImGuiWindowFlags flags)
	{
		if (ImGui::Begin(label, opened, flags))
		{
			// cant perfectly know when lerp ended, this is enough
			if(const auto size = ImGui::GetWindowSize(); size.x > 35 && size.y > 35)
				io.IniFilename = menu.m_iniFile.c_str();

			ImGui::GetForegroundDrawList()->AddCircleFilled(ImGui::GetWindowPos(), 10, ImColor{ 255, 60, 255, 255 }, 18);

			const bool sizeChanged = isResizedWindow();
			const bool posChanged = isMovedWindow();

			if (sizeChanged || posChanged)
			{
				itr->second.targetSize = ImGui::GetWindowSize();
				itr->second.pos = ImGui::GetWindowPos() + itr->second.targetSize / 2;

				ImGui::SaveIniSettingsToDisk(menu.m_iniFile.c_str());
			}

			callback();

			ImGui::End();
		}
	};

#ifdef _DEBUG
	//ImGui::GetForegroundDrawList()->AddCircleFilled(itr->second.pos, 10, ImColor{ 0, 60, 255, 255 }, 18);
#endif

	ImGuiStyle& style = ImGui::GetStyle();
	//const ImGuiStyle& backupStyle = style;

	constexpr float min = 0.0f, max = 1.0f;
	const float wantedAlpha = *opened ? max : min;
	//const float step = ImGui::GetIO().DeltaTime * 4.0f;
	itr->second.alpha = ImLerp(itr->second.alpha, wantedAlpha, ImGui::GetIO().DeltaTime * 10.0f);
	//itr->second.alpha = ImClamp(itr->second.alpha + (*opened ? step : -step), min, max);
	//itr->second.size = itr->second.targetSize * itr->second.alpha
	const ImVec2 target = *opened ? itr->second.targetSize : ImVec2{ 0, 0 };
	// 14 is passed to accelrate the lerping steps, so the inTransmission check can work well enough
	itr->second.size = ImLerp(itr->second.size, target, ImGui::GetIO().DeltaTime * 14.0f);

	ImGui::GetForegroundDrawList()->AddCircleFilled(itr->second.pos, 10, ImColor{ 0, 60, 255, 255 }, 18);

	if (inTransmission(min, max))
	{
		io.IniFilename = nullptr;

		for (int i = 0; i < ImGuiCol_COUNT; ++i)
		{
			ImVec4 color = style.Colors[i];
			color.w *= itr->second.alpha;
			ImGui::PushStyleColor(i, color);
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, style.WindowBorderSize * itr->second.alpha);
		ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, style.ScrollbarSize * itr->second.alpha);

		ImGui::SetNextWindowPos(itr->second.pos, ImGuiCond_Always, ImVec2{ 0.5f, 0.5f });
		ImGui::SetNextWindowSize(itr->second.size);

		renderGui(ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

		ImGui::PopStyleColor(ImGuiCol_COUNT);

		ImGui::PopStyleVar(2);
		return;
	}
	if (*opened)
	{
		if (newWindow)
		{
			ImGui::SetNextWindowPos(itr->second.pos, ImGuiCond_Once);
			ImGui::SetNextWindowSize(itr->second.size);
		}

		renderGui(config.flags);
	}
}