#pragma once

#include "material.hpp"

#include <utilities/tools/tools.hpp>
#include "helpers/pad.hpp"
#include "helpers/vfunc.hpp"

#include <string>

class IVStudioRender
{
    PAD(592);
private:
    IMaterial* m_material;
    PAD(12);
private:
    OverrideType_t m_type;
public:
    VFUNC(void, forcedMaterialOverride, 33, (IMaterial* mat, OverrideType_t type = OverrideType_t::OVERRIDE_NORMAL, int index = -1), (this, mat, type, index));
    bool isForced()
    {
        return (!m_material)
            ? m_type == OverrideType_t::OVERRIDE_DEPTH_WRITE || m_type == OverrideType_t::OVERRIDE_SSAO_DEPTH_WRITE
            : std::string_view(m_material->getName()).starts_with("dev/glow")
            ;
    }
};