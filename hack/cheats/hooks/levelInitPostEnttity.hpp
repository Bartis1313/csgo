#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class LevelInitPostEntity
{
public:
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR);

	static value FASTCALL hook(FAST_ARGS);
	inline static call original;

	using clRun = std::function<void()>;
protected:
	void registerPostReset(const clRun& callback) { Storage::runs.push_back(callback); }
private:
	struct Storage
	{
		inline static vectorRunnable<clRun> runs;
	};
};
_HOOK_END