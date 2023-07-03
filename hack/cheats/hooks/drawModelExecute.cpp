#include "drawModelExecute.hpp"

#include <cheats/features/visuals/chams/chams.hpp>
#include <config/vars.hpp>

#include <cheats/features/visuals/glow/glow.hpp>
#include <SDK/material.hpp>
#include <SDK/IMaterialSystem.hpp>
#include <SDK/KeyValuesSys.hpp>
#include <SDK/KeyValues.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/IVModelRender.hpp>
#include <SDK/IVModelInfo.hpp>
#include <SDK/IVStudioRender.hpp>
#include <SDK/IMatRenderContext.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/materialInit.hpp>
#include <SDK/structs/Entity.hpp>
#include <cheats/game/game.hpp>

hooks::DrawModelExecute::value hooks::DrawModelExecute::hook(FAST_ARGS, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix)
{
	if (globals::isShutdown)
	{
		static std::once_flag onceFlag;
		std::call_once(onceFlag, []() { Storage::shutdowns.run(); });
		return original(thisptr, ctx, state, info, matrix);
	}

	static std::once_flag onceFlag;
	std::call_once(onceFlag, []() { Storage::inits.run(); });

	if (memory::interfaces::modelRender->isForcedMaterial())
		return original(thisptr, ctx, state, info, matrix);

	if (chams::DME::run(ctx, state, info, matrix))
		return;

	if (chams::DME::shouldDisableDME(info))
		return;

	original(thisptr, ctx, state, info, matrix);

	memory::interfaces::modelRender->overrideMaterial(nullptr);
}