#pragma once

template <typename T>
class CUtlVector
{
public:
	constexpr T& operator[](int i)
	{
		return m_memory[i];
	};

	constexpr T operator[](int i) const
	{
		return m_memory[i];
	};

	T* m_memory;
	int m_sizeCount;
	int m_growSize;
	int m_size;
	T* m_elements;
};