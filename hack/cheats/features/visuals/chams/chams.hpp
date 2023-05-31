#pragma once

#include "factory/factory.hpp"
#include "../streamproof/streamproof.hpp"

#include <SDK/math/matrix.hpp>

#include <vector>

struct DrawModelState_t;
struct ModelRenderInfo_t;

// for streamproof, better to just call drawmodel
namespace chams
{
	void run(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix);
	void init();
	void initMaterials();

	inline std::vector<MaterialData> materials;
	inline StreamProof streamProof{ "chams1337" };
	inline bool runningMyChams{ false };
}