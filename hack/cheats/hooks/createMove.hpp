#pragma once

#include "helpers/common.hpp"

#include <SDK/CUserCmd.hpp>

_HOOK_BEGIN
class CreateMove
{
	ADD_HOOK_FRIEND;
private:
	using value = bool;
	using call = value(THISCALL*)(THISPTR, float, CUserCmd*);

	static value FASTCALL hook(FAST_ARGS, float inputFrame, CUserCmd* cmd);
	inline static call original;

	using clInit = std::function<void()>;
	using clRun = std::function<void(CUserCmd*)>;
	using clShutdown = std::function<void()>;
protected:
	void registerInit(const clInit& callback) { Storage::inits.push_back(callback); }
	void registerRunPrePrediction(const clRun& callback) { Storage::runsPrePrediction.push_back(callback); }
	void registerRunPrediction(const clRun& callback) { Storage::runsPrediction.push_back(callback); }
	void registerRunPostPrediction(const clRun& callback) { Storage::runsPostPrediction.push_back(callback); }
private:
	struct Storage
	{
		inline static vectorRunnable<clInit> inits;
		inline static vectorRunnable<clRun> runsPrePrediction;
		inline static vectorRunnable<clRun> runsPrediction;
		inline static vectorRunnable<clRun> runsPostPrediction;
	};
};
_HOOK_END