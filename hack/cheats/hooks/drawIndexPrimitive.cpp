#include "drawIndexPrimitive.hpp"

#include <gamememory/memory.hpp>

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

hooks::DrawIndexedPrimitive::value hooks::DrawIndexedPrimitive::hook(IDirect3DDevice9* device, D3DPRIMITIVETYPE primType, INT basevertexIndex, UINT minVertexIndex,
	UINT numVertices, UINT startIndex, UINT primCount)
{
	if (globals::isShutdown)
	{
		static std::once_flag onceFlag;
		std::call_once(onceFlag, []() { Storage::shutdowns.run(); });
		return original(device, primType, basevertexIndex, minVertexIndex, numVertices, startIndex, primCount);
	}

	void** data;
	__asm mov data, ebp
	
	auto test = getStack(data);

	static std::once_flag onceFlag;
	std::call_once(onceFlag, []() { Storage::inits.run(); });

	Storage::runs.run(device, primType, basevertexIndex, minVertexIndex, numVertices, startIndex, primCount);

	return original(device, primType, basevertexIndex, minVertexIndex, numVertices, startIndex, primCount);
}