#include "hooks.hpp"
#include "../game.hpp"
#include "../features/visuals/chams.hpp"

void __stdcall hooks::drawModel::hooked(void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix)
{
	// when invalid it will make all players invisible
	if (!game::localPlayer)
		return;

	if(!interfaces::engine->isInGame() || interfaces::modelRender->isForcedMaterial())
		return original(interfaces::modelRender, ctx, std::cref(state), std::cref(info), matrix);

	chams::run(ctx, std::cref(state), std::cref(info), matrix);
	chams::drawModel(ctx, std::cref(state), std::cref(info), matrix);
	
	original(interfaces::modelRender, ctx, std::cref(state), std::cref(info), matrix);
	interfaces::modelRender->overrideMaterial(nullptr);
}