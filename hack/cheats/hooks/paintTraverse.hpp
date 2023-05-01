#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class PaintTraverse
{
public:
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR, uint32_t, bool, bool);

	static value FASTCALL hook(FAST_ARGS, uint32_t panel, bool forceRepaint, bool allowForce);
	inline static call original;

	using clRun = std::function<void()>;
protected:
	void registerRender(const clRun& callback) { Storage::runs.push_back(callback); }
private:
	struct Storage
	{
		inline static vectorRunnable<clRun> runs;
	};
};
_HOOK_END