#pragma once

#include "factory/factory.hpp"
#include "../streamproof/streamproof.hpp"

#include <SDK/math/matrix.hpp>

#include <vector>

struct DrawModelState_t;
struct ModelRenderInfo_t;
class IMatRenderContext;

namespace chams
{
	void init();
	void initMaterials();

	namespace DME
	{
		bool shouldDisableDME(const ModelRenderInfo_t& info);
		bool run(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix);
	}

	namespace manual
	{
		void run();
	}

	inline std::vector<MaterialData> materials;
	inline StreamProof streamProof{ "chams1337" };
	inline StreamProof streamProofDME{ "chams1337dme" };
}