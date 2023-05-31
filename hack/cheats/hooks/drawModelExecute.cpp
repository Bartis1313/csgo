#include "drawModelExecute.hpp"

hooks::DrawModelExecute::value hooks::DrawModelExecute::hook(FAST_ARGS, void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix)
{
	if (globals::isShutdown)
	{
		static std::once_flag onceFlag;
		std::call_once(onceFlag, []() { Storage::shutdowns.run(); });
		return original(thisptr, result, state, info, matrix);
	}

	static std::once_flag onceFlag;
	std::call_once(onceFlag, []() { Storage::inits.run(); });

	if (memory::interfaces::modelRender->isForcedMaterial())
		return original(thisptr, result, state, info, matrix);

	Storage::runs.run(result, state, info, matrix);

	original(thisptr, result, state, info, matrix);

	memory::interfaces::modelRender->overrideMaterial(nullptr);
}