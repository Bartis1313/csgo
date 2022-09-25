#pragma once

#include "CUtlVector.hpp"

#include <utilities/pad.hpp>
#include <utilities/vfunc.hpp>

class IConVar
{
public:
	VFUNC(float, getFloat, 12, (), (this));
	VFUNC(int, getInt, 13, (), (this));
	VFUNC(void, setValue, 14, (const char* val), (this, val));
	VFUNC(void, setValue, 15, (float val), (this, val));
	VFUNC(void, setValue, 16, (int val), (this, val));
	VFUNC(void, setValue, 17, (bool val), (this, val));

	PAD(4);
	IConVar* m_next;
	int m_isRegistered;
	char* m_name;
	char* m_helper;
	int m_flags;
	PAD(4);
	IConVar* m_parent;
	char* m_defaultVal;
	char* m_string;
	int m_stringLenght;
	float m_floatVal;
	int m_numVal;
	int m_hasMIn;
	float m_min;
	int m_hasMax;
	float m_max;
	CUtlVector<void(__cdecl*)()> m_callbacks;
};