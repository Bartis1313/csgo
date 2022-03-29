#pragma once

template <typename T>
class UtlVector
{
public:
	constexpr T& operator[](int i)
	{
		return m_memory[i];
	};

	T* m_memory;
	int m_sizeCount;
	int m_growSize;
	int m_size;
	T* m_elementsCount;
};