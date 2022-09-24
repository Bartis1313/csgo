#include "hooks.hpp"

#include <SDK/CStudioHdr.hpp>
#include <SDK/CUtlVector.hpp>
#include <SDK/math/matrix.hpp>

//#include "../../SDK/CGlobalVars.hpp"
//#include "../../SDK/structs/Entity.hpp"
//#include "../../SDK/interfaces/interfaces.hpp"

// prevent spoofing convar for jiggle bones
void __fastcall hooks::buildTransformations::hooked(FAST_ARGS, CStudioHdr* hdr, void* pos, void* q, const Matrix3x4& matrix, int boneMask, void* computed)
{
    //for (int i = 1; i <= interfaces::globalVars->m_maxClients; i++)
    //{
    //    auto ent = reinterpret_cast<Entity_t*>(reinterpret_cast<uintptr_t>(thisptr) + 0x64);

    //    if (ent)
    //        continue;

    //    // and here u check flags |= 8, but there are better ways like on bottom
    //}

    CUtlVector<int> flags = hdr->m_boneFlags;

    for (int i = 0; i < hdr->m_boneFlags.m_size; i++)
    {
        hdr->m_boneFlags.m_elementsCount[i] &= ~4;
    }

    original(thisptr, hdr, pos, q, matrix, boneMask, computed);

    hdr->m_boneFlags = flags;
}