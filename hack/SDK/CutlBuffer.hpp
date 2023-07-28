#pragma once

#include "CUtlMemory.hpp"

class CByteswap
{
public:
	unsigned int m_bSwapBytes : 1;
	unsigned int m_bBigEndian : 1;
};

class CUtlBuffer
{
public:
	CUtlBuffer() = default;
	CUtlBuffer(const void* buffer, int size, int flags)
		: m_Memory((unsigned char*)buffer, size), m_Error(0)
	{
		m_Get = 0;
		m_Put = 0;
		m_nTab = 0;
		m_nOffset = 0;
		m_Flags = (unsigned char)flags;
		if (IsReadOnly()) 
		{
			m_nMaxPut = size;
		}
		else 
		{
			m_nMaxPut = -1;
			AddNullTermination();
		}
		SetOverflowFuncs(&CUtlBuffer::GetOverflow, &CUtlBuffer::PutOverflow);
	}

	enum SeekType_t
	{
		SEEK_HEAD = 0,
		SEEK_CURRENT,
		SEEK_TAIL
	};

	// flags
	enum BufferFlags_t
	{
		TEXT_BUFFER = 0x1,			// Describes how Get + put work (as strings, or binary)
		EXTERNAL_GROWABLE = 0x2,	// This is used w/ external buffers and causes the utlbuf to switch to reallocatable memory if an overflow happens when Putting.
		CONTAINS_CRLF = 0x4,		// For text buffers only, does this contain \n or \n\r?
		READ_ONLY = 0x8,			// For external buffers; prevents null termination from happening.
		AUTO_TABS_DISABLED = 0x10,	// Used to disable/enable push/pop tabs
	};

	typedef bool (CUtlBuffer::* UtlBufferOverflowFunc_t)(int nSize);

	void SetOverflowFuncs(UtlBufferOverflowFunc_t getFunc, UtlBufferOverflowFunc_t putFunc)
	{
		m_GetOverflowFunc = getFunc;
		m_PutOverflowFunc = putFunc;
	}

	void* PeekPut(int offset)
	{
		return &m_Memory[m_Put + offset - m_nOffset];
	}

	bool GetOverflow(int nSize)
	{
		return false;
	}

	inline bool IsGrowable() const
	{
		return (m_Flags & EXTERNAL_GROWABLE) != 0;
	}

	bool PutOverflow(int nSize)
	{
		if (m_Memory.IsExternallyAllocated()) 
		{
			if (!IsGrowable())
				return false;

			m_Memory.ConvertToGrowableMemory(0);
		}

		while (Size() < m_Put - m_nOffset + nSize) 
		{
			m_Memory.Grow();
		}

		return true;
	}

	int Size() const
	{
		return m_Memory.m_nAllocationCount;
	}

	bool IsReadOnly() const
	{
		return (m_Flags & READ_ONLY);
	}

	void AddNullTermination()
	{
		if (m_Put > m_nMaxPut) {
			if (!IsReadOnly() && ((m_Error & PUT_OVERFLOW) == 0)) 
			{
				// Add null termination value
				if (CheckPut(1)) 
				{
					m_Memory[m_Put - m_nOffset] = 0;
				}
				else 
				{
					// Restore the overflow state, it was valid before...
					m_Error &= ~PUT_OVERFLOW;
				}
			}
			m_nMaxPut = m_Put;
		}
	}

protected:
	// error flags
	enum
	{
		PUT_OVERFLOW = 0x1,
		GET_OVERFLOW = 0x2,
		MAX_ERROR_FLAG = GET_OVERFLOW,
	};

	bool OnPutOverflow(int nSize)
	{
		return (this->*m_PutOverflowFunc)(nSize);
	}

	bool OnGetOverflow(int nSize)
	{
		return (this->*m_GetOverflowFunc)(nSize);
	}

	bool CheckPut(const int nSize)
	{
		if ((m_Error & PUT_OVERFLOW) || IsReadOnly())
			return false;

		if ((m_Put < m_nOffset) || (m_Memory.NumAllocated() < m_Put - m_nOffset + nSize)) 
		{
			if (!OnPutOverflow(nSize)) 
			{
				m_Error |= PUT_OVERFLOW;
				return false;
			}
		}
		return true;
	}

	CUtlMemory<unsigned char> m_Memory;
	int m_Get;
	int m_Put;

	unsigned char m_Error;
	unsigned char m_Flags;
	unsigned char m_Reserved;
#if defined( _X360 )
	unsigned char pad;
#endif

	int m_nTab;
	int m_nMaxPut;
	int m_nOffset;

	UtlBufferOverflowFunc_t m_GetOverflowFunc;
	UtlBufferOverflowFunc_t m_PutOverflowFunc;

	CByteswap	m_Byteswap;
};