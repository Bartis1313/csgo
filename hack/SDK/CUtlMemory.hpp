#pragma once

inline int UtlMemory_CalcNewAllocationCount(int nAllocationCount, int nGrowSize, int nNewSize, int nBytesItem)
{
	if (nGrowSize)
		nAllocationCount = ((1 + ((nNewSize - 1) / nGrowSize)) * nGrowSize);
	else
	{
		if (!nAllocationCount)
			nAllocationCount = (31 + nBytesItem) / nBytesItem;

		while (nAllocationCount < nNewSize)
			nAllocationCount *= 2;
	}

	return nAllocationCount;
}

//-----------------------------------------------------------------------------
// The CUtlMemory class:
// A growable memory class which doubles in size by default.
//-----------------------------------------------------------------------------
template< class T, class I = int >
class CUtlMemory
{
public:
	CUtlMemory() = default;
	CUtlMemory(T* pMemory, int numElements) : m_pMemory(pMemory),
		m_nAllocationCount(numElements)
	{
		// Special marker indicating externally supplied modifyable memory
		m_nGrowSize = -1; // EXTERNAL_BUFFER_MARKER 
	}

	T& operator[](I i)
	{
		return m_pMemory[i];
	}

	T& operator[](I i) const
	{
		return m_pMemory[i];
	}

	T* Base()
	{
		return m_pMemory;
	}

	T* Base() const
	{
		return m_pMemory;
	}

	int NumAllocated() const
	{
		return m_nAllocationCount;
	}

	void Grow(int num = 1)
	{
		if (IsExternallyAllocated())
			return;

		int nAllocationRequested = m_nAllocationCount + num;
		int nNewAllocationCount = UtlMemory_CalcNewAllocationCount(m_nAllocationCount, m_nGrowSize, nAllocationRequested, sizeof(T));

		if ((int)(I)nNewAllocationCount < nAllocationRequested)
		{
			if ((int)(I)nNewAllocationCount == 0 && (int)(I)(nNewAllocationCount - 1) >= nAllocationRequested)
				--nNewAllocationCount;
			else
			{
				if ((int)(I)nAllocationRequested != nAllocationRequested)
					return;

				while ((int)(I)nNewAllocationCount < nAllocationRequested)
					nNewAllocationCount = (nNewAllocationCount + nAllocationRequested) / 2;
			}
		}

		m_nAllocationCount = nNewAllocationCount;

		if (m_pMemory)
			m_pMemory = (T*)realloc(m_pMemory, m_nAllocationCount * sizeof(T));
		else
			m_pMemory = (T*)malloc(m_nAllocationCount * sizeof(T));
	}

	bool IsExternallyAllocated() const
	{
		return m_nGrowSize < 0;
	}

	void ConvertToGrowableMemory(int nGrowSize)
	{
		if (!IsExternallyAllocated())
			return;

		m_nGrowSize = nGrowSize;
		if (m_nAllocationCount) 
		{
			int nNumBytes = m_nAllocationCount * sizeof(T);
			T* pMemory = (T*)malloc(nNumBytes);
			memcpy(pMemory, m_pMemory, nNumBytes);
			m_pMemory = pMemory;
		}
		else 
		{
			m_pMemory = NULL;
		}
	}

public:
	T* m_pMemory;
	int m_nAllocationCount;
	int m_nGrowSize;
};