#pragma once

#include "helpers/common.hpp"

#include <SDK/CViewSetup.hpp>

_HOOK_BEGIN
class Screen2DEffect
{
public:
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR, CViewSetup*);

	static value FASTCALL hook(FAST_ARGS, CViewSetup* view);
	inline static call original;

	using clInit = std::function<void()>;
	using clRun = std::function<void(CViewSetup*)>;
protected:
	void registerInit(const clInit& callback) { Storage::inits.push_back(callback); }
	void registerRun(const clRun& callback) { Storage::runs.push_back(callback); }
	void registerShutdown(const clInit& callback) { Storage::shutdowns.push_back(callback); }
private:
	struct Storage
	{
		inline static vectorRunnable<clInit> inits;
		inline static vectorRunnable<clRun> runs;
		inline static vectorRunnable<clInit> shutdowns;
	};
};
_HOOK_END