#pragma once

#include <vector>

template<typename T = float>
struct SimpleVec
{
public:
	SimpleVec() = default;
	template<typename... Args_t>
	constexpr SimpleVec(Args_t&&... args) :
		m_sizeArgs{ sizeof...(Args_t) }
	{
		m_arr.resize(m_sizeArgs);
		m_arr = { static_cast<T>(args)... }; // allow our mistakes
	}

	constexpr T& operator[](size_t idx)
	{
		return m_arr[idx];
	}
private:
	size_t m_sizeArgs = 0U;
	std::vector<T> m_arr{};
};
