#include "hooks.hpp"

// STRIDE 32 - OK

// STRIDE 32 NUMVERT 3300 PRIMCOUNT 4863 - PLAYER CT - WORKSHOP MAP
// STRIDE 32 NUMVERT 3650 PRIMCOUNT 4030 - PLAYER CT - WORKSHOP MAP
// STRIDE 32 NUMVERT 3692 PRIMCOUNT 5699 - PLAYER TT - WORKSHOP MAP
// STRIDE 32 NUMVERT 2118 PRIMCOUNT 3354 - PLAYER CT - DUST 2
// STRIDE 32 NUMVERT 3225 PRIMCOUNT 4751 - PLAYER TT - DUST 2
// STRIDE 32 NUMVERT 3742 PRIMCOUNT 5672 - PLAYER TT - DUST 2
// STRIDE 32 NUMVERT 3932 PRIMCOUNT 5551 - PLAYER TT - DUST 2 - (all tt but not head)
// ...

long __stdcall hooks::drawIndexedPrimitive::hooked(IDirect3DDevice9* device, D3DPRIMITIVETYPE primType, INT basevertexIndex, UINT minVertexIndex,
	UINT numVertices, UINT startIndex, UINT primCount)
{
	// need help here :(, I spotted that some models have diff size on maps. Which make this painfully slow to find
	// only way I have in mind is: ret addr check or use globals from csgo hooks such as: dme
	// this is only for my purposes to practise, there is nothing very wrong with built in chams

	return original(device, primType, basevertexIndex, minVertexIndex, numVertices, startIndex, primCount);
}