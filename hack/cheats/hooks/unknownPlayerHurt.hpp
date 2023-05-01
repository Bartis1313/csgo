#pragma once

#include "helpers/common.hpp"

#include <SDK/IGameEvent.hpp>

_HOOK_BEGIN
class UnknownPlayerHurt
{
public:
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR, void*);

	static value FASTCALL hook(FAST_ARGS, void* _event);
	inline static call original;

	using clRun = std::function<void(IGameEvent*)>;
protected:
	void registerRun(const clRun& callback) { Storage::runs.push_back(callback); }
private:
	struct Storage
	{
		inline static vectorRunnable<clRun> runs;
	};
};
_HOOK_END