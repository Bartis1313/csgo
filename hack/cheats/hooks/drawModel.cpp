#include "hooks.hpp"

#include "../classes/drawModel.hpp"

#include <SDK/IVModelRender.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/interfaces/interfaces.hpp>

hooks::drawModel::value FASTCALL hooks::drawModel::hooked(FAST_ARGS, void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix)
{
	if(!memory::interfaces::engine->isInGame() || memory::interfaces::modelRender->isForcedMaterial())
		return original(memory::interfaces::modelRender(), result, state, info, matrix);

	DrawModelType::runAll(result, state, info, matrix);

	original(memory::interfaces::modelRender(), result, state, info, matrix);
	memory::interfaces::modelRender->overrideMaterial(nullptr);
}