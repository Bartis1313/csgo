#pragma once

#include "helpers/common.hpp"

#include <SDK/CViewSetup.hpp>

_HOOK_BEGIN
class ViewRender
{
public:
	static auto getOriginal() { return original; }
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR, const CViewSetup&, const CViewSetup&, int, int);

	static value FASTCALL hook(FAST_ARGS, const CViewSetup& view, const CViewSetup& hud, int clearFlags, int whatToDraw);
	inline static call original;

	using clRun = std::function<void(const CViewSetup&)>;
	using clInit = std::function<void()>;
protected:
	void registerRun(const clRun& callback) { Storage::runs.push_back(callback); }
	void registerInit(const clInit& callback) { Storage::inits.push_back(callback); }
private:
	struct Storage
	{
		inline static vectorRunnable<clRun> runs;
		inline static vectorRunnable<clInit> inits;
	};
};
_HOOK_END