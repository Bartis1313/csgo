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

#include <unordered_map>

static std::unordered_map<ImGuiID, std::pair<ImVec2, ImVec2>> m_mapSizes = {};
static const char* g_GroupPanelName = nullptr;

void ImGui::BeginGroupPanel(const char* name, const ImVec2& size)
{
	g_GroupPanelName = name;

	ImGui::PushID(name);

	ImGui::BeginGroup();

	auto background = ImColor{ ImGui::GetStyleColorVec4(ImGuiCol_ChildBg) };
	ImColor secondGradient =
	{
		background.Value.x - 0.1f,
		background.Value.y - 0.1f,
		background.Value.z - 0.1f,
		background.Value.w * ImGui::GetStyle().Alpha,
	};

	auto id = ImGui::GetCurrentWindow()->GetID(g_GroupPanelName);
	auto [min, max] = m_mapSizes[id];

	ImGui::GetWindowDrawList()->AddRectFilledMultiColor(
		min, max,
		background,
		background,
		secondGradient,
		secondGradient);


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
	
	auto labelMin = ImGui::GetItemRectMin();
	auto labelMax = ImGui::GetItemRectMax();
	ImGui::SameLine(0.0f, 0.0f);
	ImGui::Dummy(ImVec2(0.0, frameHeight + itemSpacing.y));
	ImGui::BeginGroup();

	ImGui::PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
	ImGui::GetCurrentWindow()->ContentRegionRect.Max.x -= frameHeight * 0.5f;
	ImGui::GetCurrentWindow()->WorkRect.Max.x -= frameHeight * 0.5f;
	ImGui::GetCurrentWindow()->InnerRect.Max.x -= frameHeight * 0.5f;
#else
	ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x -= frameHeight * 0.5f;
#endif
	ImGui::GetCurrentWindow()->Size.x -= frameHeight;

	auto itemWidth = ImGui::CalcItemWidth();
	ImGui::PushItemWidth(ImMax(0.0f, itemWidth - frameHeight));
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

	ImVec2 halfFrame = ImVec2(frameHeight * 0.25f, frameHeight) * 0.5f;
	ImRect frameRect = ImRect(itemMin + halfFrame, itemMax - ImVec2(halfFrame.x, 0.0f));
 
	ImVec2 textSize = CalcTextSize(g_GroupPanelName);
	auto draw = ImGui::GetWindowDrawList();
	float width = frameRect.Max.x - frameRect.Min.x;
	float addonoffset = width * 0.05f;

	// for line outline
	ImVec2 min = { frameRect.Min.x - 1.0f, frameRect.Min.y - 1.0f };
	ImVec2 max = { frameRect.Max.x + 1.0f, frameRect.Max.y + 1.0f };

	constexpr float idealTextOffset = 5.0f;

	// top left before text
	draw->AddLine(min, { min.x + addonoffset, min.y }, ImColor{ 0.0f, 0.0f, 0.0f, ImGui::GetStyle().Alpha });
	ImGui::PushFont(ImFonts::tahoma14);
	// shadow
	draw->AddText({ min.x + addonoffset + idealTextOffset + 1.0f, min.y - (textSize.y / 2.0f) + 1.0f }, ImColor{ 0.0f, 0.0f, 0.0f, ImGui::GetStyle().Alpha }, g_GroupPanelName);
	// normal text
	draw->AddText({ min.x + addonoffset + idealTextOffset, min.y - (textSize.y / 2.0f) }, ImGui::GetColorU32(ImGuiCol_Text), g_GroupPanelName);
	ImGui::PopFont();
	// top after text
	draw->AddLine({ min.x + addonoffset + (idealTextOffset * 2.0f) + textSize.x, min.y }, { max.x, min.y }, ImColor{ 0.0f, 0.0f, 0.0f, ImGui::GetStyle().Alpha });
	// left line
	draw->AddLine(min, { min.x, max.y }, ImColor{ 0.0f, 0.0f, 0.0f, ImGui::GetStyle().Alpha });
	// bottom line
	draw->AddLine({ min.x, max.y }, max, ImColor{ 0.0f, 0.0f, 0.0f, ImGui::GetStyle().Alpha });
	// right line
	draw->AddLine({ max.x, min.y }, max, ImColor{ 0.0f, 0.0f, 0.0f, ImGui::GetStyle().Alpha });

	auto id = ImGui::GetCurrentWindow()->GetID(g_GroupPanelName);

	auto [itr, emplaced] = m_mapSizes.try_emplace(id, std::make_pair(frameRect.Min, frameRect.Max));
	if (!emplaced)
		itr->second = std::make_pair(frameRect.Min, frameRect.Max);

	ImGui::PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
	ImGui::GetCurrentWindow()->ContentRegionRect.Max.x += frameHeight * 0.5f;
	ImGui::GetCurrentWindow()->WorkRect.Max.x += frameHeight * 0.5f;
	ImGui::GetCurrentWindow()->InnerRect.Max.x += frameHeight * 0.5f;
#else
	ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x += frameHeight * 0.5f;
#endif
	ImGui::GetCurrentWindow()->Size.x += frameHeight;

	ImGui::Dummy(ImVec2(0.0f, 0.0f));

	ImGui::EndGroup();

	ImGui::PopID();
}

static bool arrGetterStr(void* data, int idx, const char** out)
{
	auto& v = *static_cast<std::span<std::string>*>(data);

	if (out)
		*out = v[idx].c_str();
	return true;
}

static bool arrGetter(void* data, int idx, const char** out)
{
	auto& v = *static_cast<std::span<const char*>*>(data);

	if (out)
		*out = v[idx];
	return true;
}

bool ImGui::Combo(const char* label, int* item, std::span<const char*> arr, const int width)
{
	bool ret = ImGui::Combo(label, item, arr.data(), arr.size(), width);
	return ret;
}

bool ImGui::Combo(const char* label, int* item, std::span<const std::string> arr, const int width)
{
	bool ret = ImGui::Combo(label, item, &arrGetterStr, const_cast<void*>(reinterpret_cast<const void*>(&arr)), arr.size(), width);
	return ret;
}

bool ImGui::ListBox(const char* label, int* item, std::span<const std::string> arr, const int heightItem)
{
	return ImGui::ListBox(label, item, &arrGetterStr, const_cast<void*>(reinterpret_cast<const void*>(&arr)), arr.size(), heightItem);
}

bool ImGui::ListBox(const char* label, int* item, std::span<const char*> arr, const int heightItem)
{
	return ImGui::ListBox(label, item, &arrGetter, const_cast<void*>(reinterpret_cast<const void*>(&arr)), arr.size(), heightItem);
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