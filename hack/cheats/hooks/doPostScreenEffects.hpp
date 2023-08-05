#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class DoPostScreenEffects
{
	ADD_HOOK_FRIEND;
private:
	using value = int;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, int);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, int val);
	inline static call original;

	using cl = std::function<void()>;
protected:
	void registerInit(const cl& callback) { Storage::inits.push_back(callback); }
	void registerRun(const cl& callback) { Storage::runs.push_back(callback); }
	void registerShutdown(const cl& callback) { Storage::shutdowns.push_back(callback); }
private:
	struct Storage
	{
		inline static vectorRunnable<cl> inits;
		inline static vectorRunnable<cl> runs;
		inline static vectorRunnable<cl> shutdowns;
	};
};
_HOOK_END