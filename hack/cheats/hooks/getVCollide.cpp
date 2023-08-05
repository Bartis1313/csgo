#include "getVCollide.hpp"

#include <gamememory/memory.hpp>
#include <SDK/CUtlVector.hpp>
#include <SDK/IVModelInfo.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>

#include <cheats/game/game.hpp>
#include <utilities/console/console.hpp>

#include <cheats/features/visuals/world/weather/precipitation.hpp>
#include <utilities/console/console.hpp>

#include <intrin.h>

// server modelinfo
// 8B 0D ? ? ? ? 8B 04 B0 0F BF 90 ? ? ? ? 8B 01 52 FF 50 18 85 C0 0F 84 ? ? ? ? B9 ? ? ? ? 66 85 08 0F 86 ? ? ? ? 8B 0D ? ? ? ? 8D 5D F4 8B 40 04 53 8B 11

// if we get null, we can't get rain meshes working! this is for particle type case
// so for own handling we need to provide generated vcollide
hooks::GetVCollide::value hooks::GetVCollide::hook(FAST_ARGS, int modelIndex)
{
    auto org = original(thisptr, modelIndex);
    
#if DUMP_WEATHER == true
    if (hooks::calledInitprecip)
    {
        if (org)
        {
            console::debug("COLLIDE WEATHER DUMP");
            console::debug("solidcount : {}, descSize : {}", static_cast<size_t>(org->solidCount), static_cast<size_t>(org->descSize)); // 1, 84
            console::debug("END COLLIDE WEATHER DUMP");
        }
    }
#endif


    if (reinterpret_cast<uintptr_t>(_ReturnAddress()) == memory::particleGetVCollideRet())
    {
        if (modelIndex == weather::precipitation::uniqueModelIndex)
            return &weather::precipitation::getCollide();
    }

    return org;
}