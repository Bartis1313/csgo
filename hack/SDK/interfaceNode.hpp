#pragma once

#include <type_traits>

struct InterfacesNode
{
	std::add_pointer_t<void* __cdecl()> m_createFn;
	const char* m_name;
	InterfacesNode* m_next;
};