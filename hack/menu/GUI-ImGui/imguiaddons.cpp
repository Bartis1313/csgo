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
	ImGui::PushFont(ImRender::fonts::tahoma20);
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

// leaving this, because it might be helpful
static void RenderArrow(ImDrawList* draw_list, ImVec2 pos, ImU32 col, ImGuiDir dir, float angle, float scale)
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

// because some update made stack for those items kinda different, I will use own non api-like solution to solve this
static std::unordered_map<ImGuiID, bool> stackHotkey;

bool ImGui::Hotkey(const char* label, Key* key, bool useExtended, const ImVec2& size)
{
	ImGui::PushID(label);
	if (std::strncmp(label, "##", 2))
		ImGui::TextUnformatted(label, std::strstr(label, "##"));

	ImGui::SameLine();
	const auto id = ImGui::GetID(label);

	bool isInHotkey = stackHotkey[id];

	if (isInHotkey)
	{
		ImGui::Button("...", size);
		ImGui::SetItemAllowOverlap();

		if (key->checkKey())
		{
			globals::isInHotkey = false;
			stackHotkey[id] = false;
			ImGui::ClearActiveID();
		}

	}
	else if (ImGui::Button(utilities::getKeyName(key->getKeyCode()).c_str(), size))
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

void ImGui::Window(const char* label, bool* opened, const ImGui::WindowConfig& config, const std::function<void()>& callback)
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
			/*if (const auto size = ImGui::GetWindowSize(); size.x > 35 && size.y > 35)
				io.IniFilename = menu.m_iniFile.c_str();*/

			ImGui::GetForegroundDrawList()->AddCircleFilled(ImGui::GetWindowPos(), 10, ImColor{ 255, 60, 255, 255 }, 18);

			const bool sizeChanged = isResizedWindow();
			const bool posChanged = isMovedWindow();

			if (sizeChanged || posChanged)
			{
				itr->second.targetSize = ImGui::GetWindowSize();
				itr->second.pos = ImGui::GetWindowPos() + itr->second.targetSize / 2;

				//ImGui::SaveIniSettingsToDisk(menu.m_iniFile.c_str());
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