#pragma once

#include "CEffectData.hpp"

using clientEffectCallback = std::add_pointer_t<void __cdecl(const CEffectData& data)>;

struct CClientEffectRegistration
{
	CClientEffectRegistration() = default;
	CClientEffectRegistration(const char* effectName, clientEffectCallback fn)
		: m_effectName{ effectName }, m_function{ fn }
	{}
	const char* m_effectName;
	clientEffectCallback m_function;
	CClientEffectRegistration* m_next;
};