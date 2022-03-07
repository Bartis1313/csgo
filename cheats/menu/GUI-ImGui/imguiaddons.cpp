#include "imguiaddons.hpp"
#include <Windows.h>
#include <array>

constexpr std::array badKeys =
{
    VK_ESCAPE, VK_RETURN,
    VK_INSERT, VK_PRINT,
    0x5B, 0x5C
};

#include "../../../utilities/utilities.hpp"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../../../dependencies/ImGui/imgui_internal.h"

bool checkKey(int* key)
{

    for (const auto& el : badKeys)
    {
        if (ImGui::IsKeyPressed(el))
            return true;
    }

    // imgui has many key handlers, so mouse is not treated like keyboard
    // 0=left, 1=right, 2=middle + extras (ImGuiMouseButton_COUNT == 5)
    for (int i = 0; i < IM_ARRAYSIZE(ImGui::GetIO().MouseDown); i++)
    {
        if (ImGui::IsMouseClicked(i))
        {
            // need this switch because imgui has own bool arr
            switch (i)
            {
            case 0:
                *key = VK_LBUTTON;
                break;
            case 1:
                *key = VK_RBUTTON;
                break;
            case 2:
                *key = VK_MBUTTON;
                break;
            case 3:
                *key = VK_XBUTTON1;
                break;
            case 4:
                *key = VK_XBUTTON2;
                break;
            default:
                break;
            }
            return true;
        }
    }

    for (int i = 0; i < IM_ARRAYSIZE(ImGui::GetIO().KeysDown); i++)
    {
        if (ImGui::IsKeyPressed(i))
        {
            *key = i;
            return true;
        }
    }

    return false;
}

void ImGui::Hotkey(const char* label, int* key, const ImVec2& size)
{
    PushID(label);
    Text(label);

    SameLine();

    if (const auto id = GetID(label); GetActiveID() == id)
    {
        Button(XOR("..."), size);
        GetCurrentContext()->ActiveIdAllowOverlap = true;

        if ((!IsItemHovered() && GetIO().MouseClicked[0]) || checkKey(key))
            ClearActiveID();
    }
    else if (Button(utilities::getKeyName(*key).c_str(), size))
        SetActiveID(id, GetCurrentWindow());

    PopID();
}

void ImGui::HelpMarker(const char* desc)
{
    ImGui::TextDisabled(XOR("(?)"));
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

#include "../../../SDK/Color.hpp"

bool ImGui::ColorPicker(const char* label, Color* clr, const ImGuiColorEditFlags flags)
{
    if (float c[] = { clr->r(), clr->g(), clr->b(), clr->a() };
        ImGui::ColorEdit4(label, c, flags))
    {
        *clr = Color(
            c[0],
            c[1],
            c[2],
            c[3]);
        return true;
    }
    return false;
}

static ImVector<ImRect> s_GroupPanelLabelStack;

void ImGui::BeginGroupPanel(const char* name, const ImVec2& size)
{
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
    ImRect frameRect = ImRect(itemMin + halfFrame, itemMax - ImVec2(halfFrame.x, 0.0f));
    labelRect.Min.x -= itemSpacing.x;
    labelRect.Max.x += itemSpacing.x;
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
            ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)),
            halfFrame.x);

        ImGui::PopClipRect();
    }

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
}

bool ImGui::Combo(const char* label, int* item, const std::span<const char*>& arr, const float width)
{
    ImGui::PushItemWidth(width);
    bool ret = ImGui::Combo(label, item, arr.data(), arr.size());
    ImGui::PopItemWidth();
    return ret;
}

static bool vecGetter(void* data, int idx, const char** out)
{
    auto& v = *static_cast<std::vector<std::string>*>(data);

    if (out)
        *out = v.at(idx).c_str();
    return true;
}

bool ImGui::ListBox(const char* label, int* item, const std::vector<std::string>& arr, const int heightItem)
{
    return ImGui::ListBox(label, item, vecGetter, const_cast<void*>(reinterpret_cast<const void*>(&arr)), arr.size(), heightItem);
}