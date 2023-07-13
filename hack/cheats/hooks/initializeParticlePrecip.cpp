#include "initializeParticlePrecip.hpp"

#include <gamememory/memory.hpp>
#include <SDK/CUtlVector.hpp>
#include <SDK/IVModelInfo.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>
#include <SDK/ICollideable.hpp>
#include <SDK/CGameRules.hpp>

#include <utilities/console/console.hpp>
#include <cheats/features/visuals/world/weather.hpp>

hooks::InitializeParticlePrecip::value hooks::InitializeParticlePrecip::hook(FAST_ARGS)
{
    using CClient_Precipitation = Entity_t;

#if DUMP_WEATHER == true
    console::debug("PRECIP COORDS DUMP");
#endif

    for (int i = 0; i < memory::gPrecipitations().m_size; i++)
    {
        CClient_Precipitation* volume = memory::gPrecipitations()[i];

#if DUMP_WEATHER == true
        auto coll = volume->collideable();
        if (coll)
        {
            console::debug("g_Precipitations[{}]", i);
            console::debug("{");
            console::debug("\tMaxs {} {} {}\tMins {} {} {}", volume->m_vecMaxs()[0], volume->m_vecMaxs()[1], volume->m_vecMaxs()[2], volume->m_vecMins()[0], volume->m_vecMins()[1], volume->m_vecMins()[2]);
            console::debug("\tOBBMaxs {} {} {}\tOBBMins {} {} {}", coll->OBBMaxs()[0], coll->OBBMaxs()[1], coll->OBBMaxs()[2], coll->OBBMins()[0], coll->OBBMins()[1], coll->OBBMins()[2]);
            console::debug("}");
        }
#endif
    }

#if DUMP_WEATHER == true
    console::debug("END PRECIP COORDS DUMP");
#endif

    original(thisptr);
}