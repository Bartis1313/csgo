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
	using clInit = std::function<void()>;
protected:
	void registerRender(const clRun& callback) { Storage::runs.push_back(callback); }
	void registerInit(const clRun& callback) { Storage::inits.push_back(callback); }
	void registerShutdown(const clRun& callback) { Storage::shutdowns.push_back(callback); }
private:
	struct Storage
	{
		inline static vectorRunnable<clRun> runs;
		inline static vectorRunnable<clRun> inits;
		inline static vectorRunnable<clRun> shutdowns;
	};
};
_HOOK_END