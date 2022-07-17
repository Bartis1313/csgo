#include "hooks.hpp"

#include "../../SDK/CGlobalVars.hpp"
#include "../../SDK/interfaces/interfaces.hpp"
#include "../../utilities/utilities.hpp"
#include "../../SDK/structs/IDXandPaterrns.hpp"
#include "../../config/vars.hpp"

bool __fastcall hooks::isHltv::hooked(FAST_ARGS)
{
    uintptr_t ent;
    __asm mov ent, edi

    const static auto occlusion = utilities::patternScan(CLIENT_DLL, SETUP_OCCLUSION);
    const static auto velocity = utilities::patternScan(CLIENT_DLL, SETUP_VELOCITY);
    const static auto accumulate = utilities::patternScan(CLIENT_DLL, ACCUMULATE_LAYERS);
    const uintptr_t ret = reinterpret_cast<uintptr_t>(_ReturnAddress());

    if (ret == occlusion)
    {
        // no need to set those values for normal view
        if (config.get<bool>(vars.bMirrorCam) && ent)
        {
            /**(_DWORD*)(ent + 0xA24) = -1;
            *(_DWORD*)(ent + 0xA30) = *(_DWORD*)(globalvarsPTR + 4);
            *(_DWORD*)(ent + 0xA2C) = *(_DWORD*)(ent + 0xA28);
            *(_DWORD*)(ent + 0xA28) = 0;*/

            *reinterpret_cast<int*>(ent + 0xA24) = -1;
            *reinterpret_cast<int*>(ent + 0xA30) = interfaces::globalVars->m_frameCount;
            *reinterpret_cast<int*>(ent + 0xA2C) = *reinterpret_cast<int*>(ent + 0xA28);
            *reinterpret_cast<int*>(ent + 0xA28) = 0;

            return true;
        }
    }

    // skip layers & setup velocity -> or EFL_DIRTY_ABSANGVELOCITY and set direct vel not interpolated
    if (ret == velocity || ret == accumulate)
        return true;
    
    return original(thisptr);
}