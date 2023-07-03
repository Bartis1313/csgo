#pragma once

#include "CUtlMemory.hpp"

#include <cassert>

template <class T>
inline T* Construct(T* pMemory)
{
	return ::new(pMemory) T;
}

template <class T>
inline void Destruct(T* pMemory)
{
	pMemory->~T();
}

template <class T>
inline T* CopyConstruct(T* pMemory, T const& src)
{
	return ::new(pMemory) T(src);
}

template<class T, class A = CUtlMemory<T>>
class CUtlVector
{
	typedef A CAllocator;
public:
	CAllocator m_memory;
	int m_size;
	T* m_elements;

	T& operator[](int i)
	{
		return m_memory[i];
	}

	T& operator[](int i) const
	{
		return m_memory[i];
	}

	T& Element(int i)
	{
		return m_memory[i];
	}

	T& Element(int i) const
	{
		return m_memory[i];
	}

	T* Base()
	{
		return m_memory.Base();
	}

	int Count() const
	{
		return m_size;
	}

	void RemoveAll()
	{
		for (int i = m_size; --i >= 0;)
			Destruct(&Element(i));

		m_size = 0;
	}

	int AddToTail()
	{
		return InsertBefore(m_size);
	}

	int AddToTail(const T& src)
	{
		// Can't insert something that's in the list... reallocation may hose us
		assert((Base() == NULL) || (&src < Base()) || (&src >= (Base() + Count())));
		return InsertBefore(m_size, src);
	}

	bool IsValidIndex(int i) const
	{
		return (i >= 0) && (i < m_size);
	}

	int InsertBefore(int elem)
	{
		GrowVector();
		ShiftElementsRight(elem);
		Construct(&Element(elem));

		return elem;
	}

	int InsertBefore(int elem, const T& src)
	{
		assert((Base() == NULL) || (&src < Base()) || (&src >= (Base() + Count())));
		assert((elem == Count()) || IsValidIndex(elem));

		GrowVector();
		ShiftElementsRight(elem);
		CopyConstruct(&Element(elem), src);
		return elem;
	}

	int GetOffset(const T& src) const
	{
		for (int i = 0; i < Count(); ++i)
		{
			if (Element(i) == src)
				return i;
		}
		return -1;
	}

	void Remove(int elem)
	{
		Destruct(&Element(elem));
		ShiftElementsLeft(elem);
		--m_size;
	}

	bool FindAndRemove(const T& src)
	{
		int elem = GetOffset(src);
		if (elem != -1)
		{
			Remove(elem);
			return true;
		}
		return false;
	}

protected:
	void GrowVector(int num = 1)
	{
		if (m_size + num > m_memory.NumAllocated())
			m_memory.Grow(m_size + num - m_memory.NumAllocated());

		m_size += num;
		ResetDbgInfo();
	}

	void ShiftElementsRight(int elem, int num = 1)
	{
		int numToMove = m_size - elem - num;
		if ((numToMove > 0) && (num > 0))
			memmove(&Element(elem + num), &Element(elem), numToMove * sizeof(T));
	}

	void ShiftElementsLeft(int elem, int num = 1)
	{
		int numToMove = m_size - elem - num;
		if ((numToMove > 0) && (num > 0))
			memmove(&Element(elem), &Element(elem + num), numToMove * sizeof(T));
	}

	inline void ResetDbgInfo()
	{
		m_elements = Base();
	}
};