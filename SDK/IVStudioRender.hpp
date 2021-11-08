#pragma once

#include <cstddef>
#include <string>
#include "material.hpp"
#include "../utilities/utilities.hpp"

class IVStudioRender
{
private:
    // no PAD because fail with public what I don't want
    char pad0[592];
    IMaterial* materialOverride;
    char pad1[12];
    OverrideType_t ovverideType;
public:
    bool isForced()
    {
        return (!materialOverride)
            ? ovverideType == OverrideType_t::DepthWrite || ovverideType == OverrideType_t::SsaoDepthWrite
            : std::string(materialOverride->getName()).starts_with(XOR("dev/glow"))
            ;
    }
};