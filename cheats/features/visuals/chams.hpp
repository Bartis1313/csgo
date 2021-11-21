#pragma once
#include "../../../SDK/structs/Entity.hpp"
#include "../../hooks/hooks.hpp"

namespace chams
{
	void ovverride(bool ignore, bool wireframe, Color color);
	void run(void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix);
	void drawChams(Player_t* ent, void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix);
	void drawBacktrackChams(Player_t* ent, void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info);
	void drawModel(void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix);
}