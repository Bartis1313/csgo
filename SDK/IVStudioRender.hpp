#pragma once
#include <string>

#include "material.hpp"

#include "../utilities/utilities.hpp"
#include "../utilities/pad.hpp"

class IVStudioRender
{
    PAD(592);
private:
    IMaterial* materialOverride;
    PAD(12);
private:
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