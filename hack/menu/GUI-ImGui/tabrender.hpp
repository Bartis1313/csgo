#pragma once

#include <functional>

struct TabRender
{
	TabRender() = default;
	TabRender(const char* name, const std::function<void()>& func)
		: m_name{ name }, m_func{ func }
	{}
	TabRender(const char* name)
		: m_name{ name }, m_func{ nullptr }
	{}

	[[nodiscard]] bool funcExist() const
	{
		return m_func != nullptr;
	}

	const char* m_name;
	std::function<void()> m_func;
};