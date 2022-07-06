#include "hooks.hpp"

#include "../../utilities/utilities.hpp"
#include "../../SDK/structs/Entity.hpp"
#include "../../SDK/IClientEntityList.hpp"
#include "../../SDK/IVEngineClient.hpp"

static void* getStack(void** data)
{
	if (!data)
		return nullptr;

	void** next = *reinterpret_cast<void***>(data);

	const static auto retAddr = reinterpret_cast<void*>(utilities::patternScan(STUDIORENDER_DLL, R_STUDIODRAWPOINTS));
	if (data[1] == retAddr)
		return next[4];

	return getStack(next);
}

static Player_t* getPlayer()
{
	void** data = nullptr;
	__asm mov data, ebp
	return reinterpret_cast<Player_t*>(getStack(data));
}

long __stdcall hooks::drawIndexedPrimitive::hooked(IDirect3DDevice9* device, D3DPRIMITIVETYPE primType, INT basevertexIndex, UINT minVertexIndex,
	UINT numVertices, UINT startIndex, UINT primCount)
{
	auto res = original(device, primType, basevertexIndex, minVertexIndex, numVertices, startIndex, primCount);

	auto ent = getPlayer();
	if (!ent)
		return res;

	auto local = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(interfaces::engine->getLocalPlayer()));
	if (!local)
		return res;

	if (ent == local)
		return res;

	if (!ent->isAlive())
		return res;

	if (local->m_iTeamNum() != ent->m_iTeamNum())
	{

		// draw here

		// return true;
	}

	return res;
}