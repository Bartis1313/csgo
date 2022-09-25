#pragma once

#include "material.hpp"

#include <utilities/tools/tools.hpp>
#include <utilities/pad.hpp>
#include <utilities/vfunc.hpp>

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
    VFUNC(void, forcedMaterialOverride, 33, (IMaterial* mat, OverrideType_t type = Normal, int index = -1), (this, mat, type, index));
    bool isForced()
    {
        return (!m_material)
            ? m_type == DepthWrite || m_type == SsaoDepthWrite
            : std::string_view(m_material->getName()).starts_with(XOR("dev/glow"))
            ;
    }
};