#include "hooks.hpp"

#include "../../SDK/IVModelRender.hpp"
#include "../../SDK/IVEngineClient.hpp"
#include "../../SDK/interfaces/interfaces.hpp"

#include "../features/visuals/chams.hpp"

void __stdcall hooks::drawModel::hooked(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix)
{
	if(!interfaces::engine->isInGame() || interfaces::modelRender->isForcedMaterial())
		return original(interfaces::modelRender, result, state, info, matrix);

	//chams::run(ctx, state, info, matrix);
	//chams::drawModel(ctx, state, info, matrix);
	chams.run(result, state, info, matrix);
	
	original(interfaces::modelRender, result, state, info, matrix);
	interfaces::modelRender->overrideMaterial(nullptr);
}