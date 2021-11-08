#pragma once
#include "../../hooks/hooks.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"

namespace chams
{
	void ovverride(bool, bool, Color);

	void run(void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix);

}