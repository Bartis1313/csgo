#pragma once

#include "UtlVector.hpp"
#include "../utilities/pad.hpp"

struct Studiohdr_t;
struct Virtualmodel_t;

struct CStudioHdr
{
	const Studiohdr_t* m_studioHdr;
	Virtualmodel_t* m_vModel;
	/*const virtualmodel_t* ResetVModel(const virtualmodel_t* pVModel) const;
	const studiohdr_t* GroupStudioHdr(int group);*/
	UtlVector<const Studiohdr_t*> m_studioHdrCache;
	int m_frameUnlockCounter;
	int* m_frameUnlockCounter;
	/*CThreadFastMutex	m_FrameUnlockCounterMutex;*/
	PAD(8);
	UtlVector<int> m_boneFlags;
	UtlVector<int> m_boneParent;
};