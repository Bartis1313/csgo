#include "hooks.hpp"

long __stdcall hooks::drawIndexedPrimitive::hooked(IDirect3DDevice9* device, D3DPRIMITIVETYPE primType, INT basevertexIndex, UINT minVertexIndex,
	UINT numVertices, UINT startIndex, UINT primCount)
{
	// need help here :(, I spotted that some models have diff size on maps. Which make this painfully slow to find
	// only way I have in mind is: ret addr check or use globals from csgo hooks such as: dme
	// this is only for my purposes to practise, there is nothing very wrong with built in chams

	return original(device, primType, basevertexIndex, minVertexIndex, numVertices, startIndex, primCount);
}