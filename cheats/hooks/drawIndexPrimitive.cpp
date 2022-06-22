#include "hooks.hpp"

#include <functional>

#include "../../utilities/utilities.hpp"
#include "../../SDK/structs/Entity.hpp"
#include "../../SDK/IClientEntityList.hpp"
#include "../../SDK/IVEngineClient.hpp"

class EntStack
{
public:
	Player_t* getPlayer()
	{
		void** data;
		__asm mov data, ebp
		return reinterpret_cast<Player_t*>(data);
	}
private:
	void* getStack(void** data)
	{
		if (!data)
			return nullptr;

		const static auto retAddr = utilities::patternScan(STUDIORENDER_DLL, R_STUDIODRAWPOINTS);
		void** next = *reinterpret_cast<void***>(data);

		if (reinterpret_cast<uintptr_t>(data[1]) == retAddr)
			return next[4];

		return getStack(data);
	}
};

long __stdcall hooks::drawIndexedPrimitive::hooked(IDirect3DDevice9* device, D3DPRIMITIVETYPE primType, INT basevertexIndex, UINT minVertexIndex,
	UINT numVertices, UINT startIndex, UINT primCount)
{
	auto res = original(device, primType, basevertexIndex, minVertexIndex, numVertices, startIndex, primCount);

	EntStack eStack;
	auto ent = eStack.getPlayer();
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