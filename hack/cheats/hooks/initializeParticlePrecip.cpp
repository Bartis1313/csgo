#include "initializeParticlePrecip.hpp"

#if UNLOCK_PRECIP_TESTING == true

#include <gamememory/memory.hpp>
#include <SDK/CUtlVector.hpp>
#include <SDK/IVModelInfo.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>

hooks::InitializeParticlePrecip::value hooks::InitializeParticlePrecip::hook(FAST_ARGS)
{
    static auto g_Precipitations = **reinterpret_cast<CUtlVector<Entity_t*>**>(memory::scan(CLIENT_DLL,
        SIG("A1 ? ? ? ? 8B 0D ? ? ? ? 8B 04 B0 0F BF 90 ? ? ? ? 8B 01 52 FF 50 18 85 C0 0F 84 ? ? ? ? B9 ? ? ? ? 66 85 08 0F 86 ? ? ? ? 8B 0D ? ? ? ? 8D 5D F4 8B 40 04 53 8B 11")).getAddr() + 1);

    static auto g_Precipitationsunkcounter = **reinterpret_cast<int**>(memory::scan(CLIENT_DLL,
        SIG("3B 35 ? ? ? ? 7C E9 A1 ? ? ? ? B9 ? ? ? ? 8B 40 34 ")).getAddr() + 2);

    using CClient_Precipitation = Entity_t;

    printf("g_Precipitations size %i\n", g_Precipitations.m_size);

    for (int i = 0; i < g_Precipitations.m_size; i++)
    {
        CClient_Precipitation* volume = g_Precipitations[i];

        // wrong, this index will point to trash
        int index = *(int*)((uintptr_t)volume + 0x258); // ok, this is netvar
        // created by map - 0x258 points to heap, always 100663305, tested on particle rain
        // created by is - points to normal index, in tat case always 0 (its invalid for the collide data for meshes)
        printf("addr %p idx %i\n", volume, index);
    }

    original(thisptr);
}
#endif