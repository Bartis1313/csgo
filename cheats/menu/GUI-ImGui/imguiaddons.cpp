#include "imguiaddons.hpp"

#include "../../../config/key.hpp"

#include "../../../utilities/utilities.hpp"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../../../dependencies/ImGui/imgui_internal.h"
#include "../../globals.hpp"

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

#include "../../../config/cfgcolor.hpp"

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
            }

            ImGui::EndChild();
        }

        ImGui::EndPopup();

    }
    ImGui::PopID();

    return toReturn;
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

bool ImGui::Combo(const char* label, int* item, std::span<const char*> arr, const float width)
{
    bool ret = ImGui::Combo(label, item, arr.data(), arr.size(), width);
    return ret;
}

bool ImGui::Combo(const char* label, int* item, std::span<const std::string> arr, const float width)
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

void ImGui::MultiCombo(const char* label, const std::span<const char*>& names, std::vector<bool>& options, const float width)
{
    bool check = names.size() != options.size() || !names.empty() || !options.empty();
    assert(check && "given size of arrays args was not equal or one of them was empty");

    size_t size = names.size(); // does not matter if you pass options size here

    ImVector<const char*> actives = {};
    for (size_t i = 0; const auto & el : options)
    {
        if (el) // if active selected
            actives.push_back(names[i]);

        i++;
    }

    std::string previewName = "";
    for (size_t i = 0; const auto & el : actives)
    {
        previewName += el;

        if (i < actives.size() - 1) // add ", " on every option but not last
            previewName += ", ";

        i++;
    }

    PushItemWidth(width);
    if (BeginCombo(label, previewName.c_str()))
    {
        for (size_t i = 0; i < size; i++)
        {
            if(Selectable(names[i], options.at(i), ImGuiSelectableFlags_DontClosePopups))
                options.at(i) = !options.at(i);
        }

        EndCombo();
    }
    PopItemWidth();
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