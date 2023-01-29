#include "hooks.hpp"

#include <d3dx9.h>
#include <intrin.h>

#include "../features/visuals/radar/radar.hpp"

#include <SDK/interfaces/interfaces.hpp>
#include <utilities/console/console.hpp>
#include <utilities/tools/tools.hpp>
#include <gamememory/memory.hpp>

// A DWORD (magic number) containing the four character code value 'DDS ' (0x20534444).
#define DDS_HEADER 0x20534444

hooks::unknownFileSystem::value FASTCALL hooks::unknownFileSystem::hooked(FAST_ARGS, void* image)
{
	uintptr_t thisptrStack;
	__asm mov thisptrStack, ebx;

	//const static auto ret = utilities::patternScan(PANORAMA_DLL, UNK_FILESYS);

	// dunno, this had to be inited before (retAddr)
	if (image && _ReturnAddress() == memory::returnAddrRadarImage() && *reinterpret_cast<uintptr_t*>(image) == DDS_HEADER)
	{
		size_t size = *reinterpret_cast<size_t*>(thisptrStack + 0x50);
		// again broken
		/*Resource res{ image, size };
		radar.m_mapTexture = res.getTexture();*/

		if (auto hr = D3DXCreateTextureFromFileInMemory(memory::interfaces::dx9Device(), image, size, &g_Radar->getTexture()); hr == D3D_OK)
			console::info("Created map texture, size: {}", size);
		else
			console::error("Creating map texture failed, code: {}", hr);
	}

	return original(thisptr, image);
}