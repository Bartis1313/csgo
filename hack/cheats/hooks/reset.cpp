#include "reset.hpp"

#include <imgui_impl_dx9.h>

hooks::Reset::value hooks::Reset::hook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto hr = original(device, params);

	if (hr == D3D_OK)
		ImGui_ImplDX9_CreateDeviceObjects();

	return hr;
}