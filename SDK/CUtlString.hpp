#pragma once

struct CUtlString
{
	char* m_buffer;
	int m_capacity;
	int m_growSize;
	int m_length;
};