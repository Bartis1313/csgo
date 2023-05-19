#pragma once

#include "factory/factory.hpp"

#include <SDK/math/matrix.hpp>

#include <vector>

struct DrawModelState_t;
struct ModelRenderInfo_t;

namespace chams
{
	void run(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix);
	void init();
	void initMaterials();

	inline std::vector<MaterialData> materials;
}