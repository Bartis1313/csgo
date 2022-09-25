#include "hooks.hpp"

#include <gamememory/memory.hpp>
#include <SDK/structs/Entity.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/IVEngineClient.hpp>
#include <game/globals.hpp>
#include <game/game.hpp>

static void* getStack(void** data)
{
	if (!data)
		return nullptr;

	void** next = *reinterpret_cast<void***>(data);

	const static auto retAddr = g_Memory.m_renderDrawPoints();
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

	
	if (!game::localPlayer)
		return res;

	if (ent == game::localPlayer)
		return res;

	if (!ent->isAlive())
		return res;

	if (ent->isOtherTeam(game::localPlayer()))
	{

		// draw here

		// return true;
	}

	return res;
}