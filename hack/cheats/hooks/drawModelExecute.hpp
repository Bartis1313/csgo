#pragma once

#include "helpers/common.hpp"

#include <SDK/IVModelRender.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/IMatRenderContext.hpp>
#include <SDK/interfaces/interfaces.hpp>

_HOOK_BEGIN
class DrawModelExecute
{
public:
	static auto getOriginal() { return original; }
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(THISCALL*)(THISPTR, IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, Matrix3x4*);

	static value FASTCALL hook(FAST_ARGS, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix);
	inline static call original;

	using clInit = std::function<void()>;
	using clRun = std::function<void(IMatRenderContext*, const DrawModelState_t&, const ModelRenderInfo_t&, Matrix3x4*)>;
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