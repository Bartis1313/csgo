#pragma once

template<typename T>
struct CUtlReference
{
	CUtlReference* m_next;
	CUtlReference* m_prev;
	T* m_obj;
};
