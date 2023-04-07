#pragma once

#include "CUtlVector.hpp"
#include "helpers/pad.hpp"
#include "helpers/vfunc.hpp"

class IConVar
{
public:
	VFUNC(float, getFloat, 12, (), (this));
	VFUNC(int, getInt, 13, (), (this));
	VFUNC(void, setValue, 14, (const char* val), (this, val));
	VFUNC(void, setValue, 15, (float val), (this, val));
	VFUNC(void, setValue, 16, (int val), (this, val));
	VFUNC(void, setValue, 17, (bool val), (this, val));

private:
	void* sometable;
public:
	IConVar* m_next;
	alignas(4) bool registered;
	char* m_name;
	char* m_helper;
	int m_flags;
private:
	void* unkCallback;
public:
	IConVar* m_parent;
	const char* m_defaultValue;
	char* m_valueNow;
	int m_valueLength;
private:
	IConVar* parentCopy1;
	IConVar* parentCopy2;
public:
	char* valuenowCopied; // bit delayed
	int m_valueLengthCopied;
private:
	IConVar* parentCopy3;
	IConVar* parentCopy4; // wtf?
	PAD(16); // all zeros
	CUtlVector<std::add_pointer_t<void __cdecl()>> m_callbacks;
};