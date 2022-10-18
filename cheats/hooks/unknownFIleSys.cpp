#include "hooks.hpp"

#include <d3dx9.h>
#include <intrin.h>

#include "../features/visuals/radar/radar.hpp"

#include <SDK/interfaces/interfaces.hpp>
#include <utilities/console/console.hpp>
#include <utilities/tools/tools.hpp>
#include <gamememory/memory.hpp>

#define DDS_HEADER 542327876

int __fastcall hooks::unknownFileSystem::hooked(FAST_ARGS, void* image)
{
	uintptr_t thisptrStack;
	__asm mov thisptrStack, ebx;

	//const static auto ret = utilities::patternScan(PANORAMA_DLL, UNK_FILESYS);

	// dunno, this had to be inited before (retAddr)
	if (image && _ReturnAddress() == g_Memory.m_returnAddrRadarImage() && *reinterpret_cast<uintptr_t*>(image) == DDS_HEADER)
	{
		size_t size = *reinterpret_cast<size_t*>(thisptrStack + 0x50);
		// again broken
		/*Resource res{ image, size };
		radar.m_mapTexture = res.getTexture();*/

		if (auto hr = D3DXCreateTextureFromFileInMemory(interfaces::dx9Device, image, size, &g_Radar->getTexture()); hr == D3D_OK)
			LOG_INFO(XOR("Created map texture, size: {}"), size);
		else
			LOG_ERR(XOR("Creating map texture failed, code: {}"), hr);
	}

	return original(thisptr, image);
}