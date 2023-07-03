#include "getVCollide.hpp"

#if UNLOCK_PRECIP_TESTING == true

#define MAX_MODEL_INDEX_BITS	12   // sent as a short
#define	MAX_MODELS				(1<<MAX_MODEL_INDEX_BITS)

#include <gamememory/memory.hpp>
#include <SDK/CUtlVector.hpp>
#include <SDK/IVModelInfo.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>

#include <cheats/game/game.hpp>
#include <utilities/console/console.hpp>

// server modelinfo
// 8B 0D ? ? ? ? 8B 04 B0 0F BF 90 ? ? ? ? 8B 01 52 FF 50 18 85 C0 0F 84 ? ? ? ? B9 ? ? ? ? 66 85 08 0F 86 ? ? ? ? 8B 0D ? ? ? ? 8D 5D F4 8B 40 04 53 8B 11

// if we get null, we can't get rain meshes working! this is for particle type case
// so for own handling we need to provide solids... uhhh how? We know we can get this from map info, but is there any elegant solution for this?
hooks::GetVCollide::value hooks::GetVCollide::hook(FAST_ARGS, int modelIndex)
{
	IVModelInfo* modelInfo = static_cast<IVModelInfo*>(thisptr);

	//printf("Index particle precip %i\n", modelIndex);

	if (hooks::calledInitprecip)
	{
		printf("index particle precip %i\n", modelIndex);
	}

	//if (hooks::calledInitprecip)
	//{
	//	// no idea why 9, probably models load in same same way everytime in game
	//	// 9 - tested particle rain
	//	auto precipmodel = modelInfo->getModel(9);

	//	console::debug("{} {} {} {} {}\t {} {} {} {} {} {}",
	//		precipmodel->m_name,
	//		precipmodel->m_serverCount,
	//		precipmodel->m_loadFlags,
	//		precipmodel->m_type,
	//		precipmodel->m_flags,
	//		precipmodel->m_vecMins[0],
	//		precipmodel->m_vecMins[1],
	//		precipmodel->m_vecMins[2],
	//		precipmodel->m_vecMaxs[0],
	//		precipmodel->m_vecMaxs[1],
	//		precipmodel->m_vecMaxs[2]
	//		);

	//	if (precipmodel)
	//	{			
	//		auto modelindexvfunc = modelInfo->getModelIndex(precipmodel->m_name);
	//		modelIndex = modelindexvfunc;
	//	}
	//}

	auto ret = original(thisptr, modelIndex);

	if (ret && modelIndex == 9)
	{
		printf("solidCount %i\n", ret->solidCount);
		printf("addr solids %p\n", ret->solids[0]);
	}


	if (modelIndex > MAX_MODELS)
	{
		printf("index out of models caching %i\n", modelIndex);
	}

	return ret;
}
#endif