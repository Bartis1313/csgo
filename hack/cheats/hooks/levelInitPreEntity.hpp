#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class LevelInitPreEntity
{
public:
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(HACK_THISCALL*)(HACK_THISPTR, const char*);

	static value HACK_FASTCALL hook(HACK_FAST_ARGS, const char*);
	inline static call original;

	using clRun = std::function<void()>;
protected:
	void registerReset(const clRun& callback) { Storage::runs.push_back(callback); }
private:
	struct Storage
	{
		inline static vectorRunnable<clRun> runs;
	};

};
_HOOK_END