#include "hooks.hpp"

#include <gamememory/memory.hpp>
#include <SDK/structs/Entity.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/IVEngineClient.hpp>
#include <cheats/game/game.hpp>
#include <utilities/console/console.hpp>

// not placed in SDK, unique use-case
//struct MaterialEbp
//{
//private:
//	void* context1; // 2, probs due to 2 textures in here
//	void* context2;
//	void* stack; // is present before material is not null
//public:
//	IMaterial* material;
//private:
//	void* dynamicProp; // attachment
//	void* unkStack;
//	int unkMaybeInit;
//	void* texture1;
//	void* texture2;
//	void* heapAfterTextureCleanup;
//};

// data[0] - all combined? some global instance of stack materials
// data[1] - studiorender
// data[2] - materialsystem
// data[3] - world render list
// data[4] - entity
// ... many other heaps that pretty often are copies / post calls results. Don't need this here
static void* getStack(void** data)
{
	if (IsBadReadPtr(data, sizeof(void*)))
		return nullptr;

	void** next = data ? *reinterpret_cast<void***>(data) : nullptr;

	if(!next)
		return nullptr;

	const static auto retAddr = memory::renderDrawPoints();
	if (next && data[1] == retAddr)
	{
		/*auto m = *reinterpret_cast<IMaterial**>((uintptr_t)data[2] + 0xC);
		if (m)
		{
			printf("%s\n", m->getName());
		}*/

		return next[4];
	}

	return getStack(next);
}

hooks::drawIndexedPrimitive::value D3DAPI hooks::drawIndexedPrimitive::hooked(IDirect3DDevice9* device, D3DPRIMITIVETYPE primType, INT basevertexIndex, UINT minVertexIndex,
	UINT numVertices, UINT startIndex, UINT primCount)
{
	void** data;
	__asm mov data, ebp
	
	auto test = getStack(data);

	return original(device, primType, basevertexIndex, minVertexIndex, numVertices, startIndex, primCount);
}