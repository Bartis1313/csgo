#include "hooks.hpp"

#include <SDK/CStudioHdr.hpp>
#include <SDK/CUtlVector.hpp>
#include <SDK/math/matrix.hpp>
#include <SDK/vars.hpp>

//#include "../../SDK/CGlobalVars.hpp"
//#include "../../SDK/structs/Entity.hpp"
//#include "../../SDK/interfaces/interfaces.hpp"

// prevent spoofing convar for jiggle bones
hooks::buildTransformations::value FASTCALL hooks::buildTransformations::hooked(FAST_ARGS, CStudioHdr* hdr, void* pos, void* q, const Matrix3x4& matrix, int boneMask, void* computed)
{
	CUtlVector<int> flags = hdr->m_boneFlags;
	for (int i = 0; i < hdr->m_boneFlags.m_size; i++)
	{
		hdr->m_boneFlags.m_elements[i] &= ~BONE_ALWAYS_PROCEDURAL;
	}

	original(thisptr, hdr, pos, q, matrix, boneMask, computed);

	hdr->m_boneFlags = flags;
}