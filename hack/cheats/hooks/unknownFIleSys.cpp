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

hooks::unknownFileSystemAlloc::value FASTCALL hooks::unknownFileSystemAlloc::hooked(FAST_ARGS, void* image)
{
	uintptr_t thisptrStack;
	__asm mov thisptrStack, ebx;

	if (image && _ReturnAddress() == memory::returnAddrRadarImage() && *reinterpret_cast<uintptr_t*>(image) == DDS_HEADER)
	{
		size_t size = *reinterpret_cast<size_t*>(thisptrStack + 0x50);

		if (auto hr = D3DXCreateTextureFromFileInMemory(memory::interfaces::dx9Device(), image, size, &g_Radar->getTexture()); hr == D3D_OK)
			console::debug("Created map texture, size: {}", size);
		else
			console::error("Creating map texture failed, code: {}", hr);
	}

	return original(thisptr, image);
}