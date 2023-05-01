#pragma once

#include "helpers/common.hpp"

#include <SDK/CViewSetup.hpp>

_HOOK_BEGIN
class OverrideView
{
public:
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR, CViewSetup*);

	static value FASTCALL hook(FAST_ARGS, CViewSetup* view);
	inline static call original;

	using clRun = std::function<void(CViewSetup*)>;
protected:
	void registerRun(const clRun& callback) { Storage::runs.push_back(callback); }
private:
	struct Storage
	{
		inline static vectorRunnable<clRun> runs;
	};
};
_HOOK_END