#include <deps/ImGui/extraDraw.hpp>

#include <imgui_internal.h>

static inline ImDrawFlags FixRectCornerFlags(ImDrawFlags flags)
{
#ifndef IMGUI_DISABLE_OBSOLETE_FUNCTIONS
    // Obsoleted in 1.82 (from February 2021)
    // Legacy Support for hard coded ~0 (used to be a suggested equivalent to ImDrawCornerFlags_All)
    //   ~0   --> ImDrawFlags_RoundCornersAll or 0
    if (flags == ~0)
        return ImDrawFlags_RoundCornersAll;

    // Legacy Support for hard coded 0x01 to 0x0F (matching 15 out of 16 old flags combinations)
    //   0x01 --> ImDrawFlags_RoundCornersTopLeft (VALUE 0x01 OVERLAPS ImDrawFlags_Closed but ImDrawFlags_Closed is never valid in this path!)
    //   0x02 --> ImDrawFlags_RoundCornersTopRight
    //   0x03 --> ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersTopRight
    //   0x04 --> ImDrawFlags_RoundCornersBotLeft
    //   0x05 --> ImDrawFlags_RoundCornersTopLeft | ImDrawFlags_RoundCornersBotLeft
    //   ...
    //   0x0F --> ImDrawFlags_RoundCornersAll or 0
    // (See all values in ImDrawCornerFlags_)
    if (flags >= 0x01 && flags <= 0x0F)
        return (flags << 4);

    // We cannot support hard coded 0x00 with 'float rounding > 0.0f' --> replace with ImDrawFlags_RoundCornersNone or use 'float rounding = 0.0f'
#endif

    // If this triggers, please update your code replacing hardcoded values with new ImDrawFlags_RoundCorners* values.
    // Note that ImDrawFlags_Closed (== 0x01) is an invalid flag for AddRect(), AddRectFilled(), PathRect() etc...
    IM_ASSERT((flags & 0x0F) == 0 && "Misuse of legacy hardcoded ImDrawCornerFlags values!");

    if ((flags & ImDrawFlags_RoundCornersMask_) == 0)
        flags |= ImDrawFlags_RoundCornersAll;

    return flags;
}

void ImGui::ExtraDrawlist::AddRectFilledMultiColor(ImDrawList* draw_list, const ImVec2& p_min, const ImVec2& p_max, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left, float rounding, ImDrawFlags flags)
{
    if (((col_upr_left | col_upr_right | col_bot_right | col_bot_left) & IM_COL32_A_MASK) == 0)
        return;

    flags = FixRectCornerFlags(flags);
    rounding = ImMin(rounding, ImFabs(p_max.x - p_min.x) * (((flags & ImDrawCornerFlags_Top) == ImDrawCornerFlags_Top) || ((flags & ImDrawCornerFlags_Bot) == ImDrawCornerFlags_Bot) ? 0.5f : 1.0f) - 1.0f);
    rounding = ImMin(rounding, ImFabs(p_max.y - p_min.y) * (((flags & ImDrawCornerFlags_Left) == ImDrawCornerFlags_Left) || ((flags & ImDrawCornerFlags_Right) == ImDrawCornerFlags_Right) ? 0.5f : 1.0f) - 1.0f);

    // https://github.com/ocornut/imgui/issues/3710#issuecomment-758555966
    if (rounding > 0.0f)
    {
        const int size_before = draw_list->VtxBuffer.Size;
        draw_list->AddRectFilled(p_min, p_max, IM_COL32_WHITE, rounding, flags);
        const int size_after = draw_list->VtxBuffer.Size;

        for (int i = size_before; i < size_after; i++)
        {
            ImDrawVert* vert = draw_list->VtxBuffer.Data + i;

            ImVec4 upr_left = ImGui::ColorConvertU32ToFloat4(col_upr_left);
            ImVec4 bot_left = ImGui::ColorConvertU32ToFloat4(col_bot_left);
            ImVec4 up_right = ImGui::ColorConvertU32ToFloat4(col_upr_right);
            ImVec4 bot_right = ImGui::ColorConvertU32ToFloat4(col_bot_right);

            float X = ImClamp((vert->pos.x - p_min.x) / (p_max.x - p_min.x), 0.0f, 1.0f);

            // 4 colors - 8 deltas

            float r1 = upr_left.x + (up_right.x - upr_left.x) * X;
            float r2 = bot_left.x + (bot_right.x - bot_left.x) * X;

            float g1 = upr_left.y + (up_right.y - upr_left.y) * X;
            float g2 = bot_left.y + (bot_right.y - bot_left.y) * X;

            float b1 = upr_left.z + (up_right.z - upr_left.z) * X;
            float b2 = bot_left.z + (bot_right.z - bot_left.z) * X;

            float a1 = upr_left.w + (up_right.w - upr_left.w) * X;
            float a2 = bot_left.w + (bot_right.w - bot_left.w) * X;


            float Y = ImClamp((vert->pos.y - p_min.y) / (p_max.y - p_min.y), 0.0f, 1.0f);
            float r = r1 + (r2 - r1) * Y;
            float g = g1 + (g2 - g1) * Y;
            float b = b1 + (b2 - b1) * Y;
            float a = a1 + (a2 - a1) * Y;
            ImVec4 RGBA(r, g, b, a);

            RGBA = RGBA * ImGui::ColorConvertU32ToFloat4(vert->col);

            vert->col = ImColor(RGBA);
        }
        return;
    }

    const ImVec2 uv = draw_list->_Data->TexUvWhitePixel;
    draw_list->PrimReserve(6, 4);
    draw_list->PrimWriteIdx((ImDrawIdx)(draw_list->_VtxCurrentIdx)); 
    draw_list->PrimWriteIdx((ImDrawIdx)(draw_list->_VtxCurrentIdx + 1));
    draw_list->PrimWriteIdx((ImDrawIdx)(draw_list->_VtxCurrentIdx + 2));
    draw_list->PrimWriteIdx((ImDrawIdx)(draw_list->_VtxCurrentIdx));
    draw_list->PrimWriteIdx((ImDrawIdx)(draw_list->_VtxCurrentIdx + 2));
    draw_list->PrimWriteIdx((ImDrawIdx)(draw_list->_VtxCurrentIdx + 3));
    draw_list->PrimWriteVtx(p_min, uv, col_upr_left);
    draw_list->PrimWriteVtx(ImVec2(p_max.x, p_min.y), uv, col_upr_right);
    draw_list->PrimWriteVtx(p_max, uv, col_bot_right);
    draw_list->PrimWriteVtx(ImVec2(p_min.x, p_max.y), uv, col_bot_left);
}

void ImGui::ExtraDrawlist::AddConvexPolyFilledMultiColor(ImDrawList* draw_list, const ImVec2* points, const ImU32* col, const int points_count)
{
    const ImVec2 uv = draw_list->_Data->TexUvWhitePixel;

    if (draw_list->Flags & ImDrawListFlags_AntiAliasedFill)
    {
        // Anti-aliased Fill
        const float AA_SIZE = 1.0f;
        const int idx_count = (points_count - 2) * 3 + points_count * 6;
        const int vtx_count = (points_count * 2);
        draw_list->PrimReserve(idx_count, vtx_count);

        // Add indexes for fill
        unsigned int vtx_inner_idx = draw_list->_VtxCurrentIdx;
        unsigned int vtx_outer_idx = draw_list->_VtxCurrentIdx + 1;
        for (int i = 2; i < points_count; i++)
        {
            draw_list->_IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx);
            draw_list->_IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + ((i - 1) << 1));
            draw_list->_IdxWritePtr[2] = (ImDrawIdx)(vtx_inner_idx + (i << 1));
            draw_list->_IdxWritePtr += 3;
        }

        // Compute normals
        ImVec2* temp_normals = (ImVec2*)alloca(points_count * sizeof(ImVec2)); //-V630
        for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
        {
            const ImVec2& p0 = points[i0];
            const ImVec2& p1 = points[i1];
            ImVec2 diff = p1 - p0;
            diff *= ImInvLength(diff, 1.0f);
            temp_normals[i0].x = diff.y;
            temp_normals[i0].y = -diff.x;
        }

        for (int i0 = points_count - 1, i1 = 0; i1 < points_count; i0 = i1++)
        {
            // Average normals
            const ImVec2& n0 = temp_normals[i0];
            const ImVec2& n1 = temp_normals[i1];
            ImVec2 dm = (n0 + n1) * 0.5f;
            float dmr2 = dm.x * dm.x + dm.y * dm.y;
            if (dmr2 > 0.000001f)
            {
                float scale = 1.0f / dmr2;
                if (scale > 100.0f) scale = 100.0f;
                dm *= scale;
            }
            dm *= AA_SIZE * 0.5f;

            // Add vertices
            draw_list->_VtxWritePtr[0].pos = (points[i1] - dm);
            draw_list->_VtxWritePtr[0].uv = uv;
            draw_list->_VtxWritePtr[0].col = col[i1];        // Inner
            draw_list->_VtxWritePtr[1].pos = (points[i1] + dm);
            draw_list->_VtxWritePtr[1].uv = uv;
            draw_list->_VtxWritePtr[1].col = col[i1] & ~IM_COL32_A_MASK;  // Outer
            draw_list->_VtxWritePtr += 2;

            // Add indexes for fringes
            draw_list->_IdxWritePtr[0] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1));
            draw_list->_IdxWritePtr[1] = (ImDrawIdx)(vtx_inner_idx + (i0 << 1));
            draw_list->_IdxWritePtr[2] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1));
            draw_list->_IdxWritePtr[3] = (ImDrawIdx)(vtx_outer_idx + (i0 << 1));
            draw_list->_IdxWritePtr[4] = (ImDrawIdx)(vtx_outer_idx + (i1 << 1)); 
            draw_list->_IdxWritePtr[5] = (ImDrawIdx)(vtx_inner_idx + (i1 << 1));
            draw_list->_IdxWritePtr += 6;
        }
        draw_list->_VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
    else
    {
        // Non Anti-aliased Fill
        const int idx_count = (points_count - 2) * 3;
        const int vtx_count = points_count;
        draw_list->PrimReserve(idx_count, vtx_count);
        for (int i = 0; i < vtx_count; i++)
        {
            draw_list->_VtxWritePtr[0].pos = points[i];
            draw_list->_VtxWritePtr[0].uv = uv;
            draw_list->_VtxWritePtr[0].col = col[i];
            draw_list->_VtxWritePtr++;
        }
        for (int i = 2; i < points_count; i++)
        {
            draw_list->_IdxWritePtr[0] = (ImDrawIdx)(draw_list->_VtxCurrentIdx);
            draw_list->_IdxWritePtr[1] = (ImDrawIdx)(draw_list->_VtxCurrentIdx + i - 1);
            draw_list->_IdxWritePtr[2] = (ImDrawIdx)(draw_list->_VtxCurrentIdx + i);
            draw_list->_IdxWritePtr += 3;
        }
        draw_list->_VtxCurrentIdx += (ImDrawIdx)vtx_count;
    }
}