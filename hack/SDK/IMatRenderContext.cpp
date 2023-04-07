#include "IMatRenderContext.hpp"

#include "math/Rect.hpp"
#include "interfaces/interfaces.hpp"
#include "IMaterialSystem.hpp"
#include <cheats/game/globals.hpp>
#include "PixEvent.hpp"

void IMatRenderContext::drawScreenEffectMaterial(IMaterial* material)
{
	/*int v11;
	__asm 
	{
		mov eax, [ebp - 10h]
		mov v11, eax
	}
	int v12;
	__asm
	{
		mov eax, [ebp - 12h]
		mov v12, eax
	}*/
	
	// v11 = height
	// v12 = width

	const static auto _call = memory::drawScreenEffectMaterial();
	__asm
	{
		push globals::screenY; v11
		xor edx, edx
		mov ecx, edi
		push globals::screenX; v12
		push 0
		xor edx, edx
		mov ecx, material
		call _call
		/*mov ecx, [esi]*/
		add esp, 12
	}
}