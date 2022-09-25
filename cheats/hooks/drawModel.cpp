#include "hooks.hpp"

#include "../classes/drawModel.hpp"

#include <SDK/IVModelRender.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/interfaces/interfaces.hpp>

void __stdcall hooks::drawModel::hooked(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix)
{
	if(!interfaces::engine->isInGame() || interfaces::modelRender->isForcedMaterial())
		return original(interfaces::modelRender, result, state, info, matrix);


	DrawModelType::runAll(result, state, info, matrix);

	original(interfaces::modelRender, result, state, info, matrix);
	interfaces::modelRender->overrideMaterial(nullptr);
}