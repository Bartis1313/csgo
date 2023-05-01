#pragma once

#include "helpers/common.hpp"

_HOOK_BEGIN
class OverrideMouse
{
public:
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR, float*, float*);

	static value FASTCALL hook(FAST_ARGS, float* x, float* y);
	inline static call original;

	using clRun = std::function<void(float*, float*)>;
	using clInit = std::function<void()>;
protected:
	void registerRun(const clRun& callback) { Storage::runs.push_back(callback); }
	void registerInit(const clInit& callback) { Storage::intis.push_back(callback); }
private:
	struct Storage
	{
		inline static vectorRunnable<clRun> runs;
		inline static vectorRunnable<clInit> intis;
	};
};
_HOOK_END