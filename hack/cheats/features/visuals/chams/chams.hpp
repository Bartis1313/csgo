#pragma once

#include "factory/factory.hpp"

#include <SDK/IVModelRender.hpp>
#include <SDK/IVModelRender.hpp>
#include <SDK/math/matrix.hpp>

#include <string>
#include <vector>

class Color;
struct DrawModelState_t;
struct ModelRenderInfo_t;
class Player_t;
class IMaterial;
class KeyValues;

namespace chams
{
	void run(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix);
	void init();
	void initMaterials();

	inline std::vector<MaterialData> materials;
}