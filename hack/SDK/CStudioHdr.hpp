#pragma once

#include "CUtlVector.hpp"
#include "IVModelInfo.hpp"

#include "helpers/pad.hpp"

struct Virtualmodel_t;

struct CStudioHdr
{
	const Studiohdr_t* m_studioHdr;
	Virtualmodel_t* m_vModel;
	/*const virtualmodel_t* ResetVModel(const virtualmodel_t* pVModel) const;
	const studiohdr_t* GroupStudioHdr(int group);*/
	CUtlVector<const Studiohdr_t*> m_studioHdrCache;
	int m_sizeframeUnlockCounter;
	int* m_frameUnlockCounter;
	/*CThreadFastMutex	m_FrameUnlockCounterMutex;*/
	PAD(8);
	CUtlVector<int> m_boneFlags;
	CUtlVector<int> m_boneParent;
};