#pragma once

#include "helpers/vfunc.hpp"

class IConVar;
struct SDKColor;

class ConCommandBase
{
public:
	VFUNC(bool, isFlagSet, 2, (int flag), (this, flag));
	VFUNC(void, addFlags, 3, (int flags), (this, flags));
	VFUNC(void, removeFlags, 4, (int flags), (this, flags));
	VFUNC(int, getFlags, 5, (), (this));
	VFUNC(ConCommandBase*, next, 9, (), (this));
	
	ConCommandBase* m_next;
	int m_registerted : 1;
	int oldFlags : 31;
	const char* name;
	const char* helper;
	int flags;
private:
	void* clb;
};

class ICvar
{
public:
	VFUNC(IConVar*, findVar, 15, (const char* name), (this, name));
	VFUNC(ConCommandBase*, findCommand, 17, (const char* name), (this, name));
	template <typename... Args_t>
	void consoleColorPrintf(const SDKColor& clr, const char* fmt, Args_t... args)
	{
		return vfunc::callVPack<void, 25, const SDKColor&, const char*, Args_t...>(this, std::cref(clr), fmt, args...);
	}
	template<typename... Args_t>
	void consolePrintf(const char* fmt, Args_t... args)
	{
		return vfunc::callVPack<void, 26, const char*, Args_t...>(this, fmt, args...);
	}

	class ICVarIteratorInternal
	{
	public:
		VFUNC(void, setFirst, 0, (), (this));
		VFUNC(void, next, 1, (), (this));
		VFUNC(bool, isValid, 2, (), (this));
		VFUNC(ConCommandBase*, get, 3, (), (this));
	};
	VFUNC(ICVarIteratorInternal*, factoryInternalIterator, 45, (), (this));
};