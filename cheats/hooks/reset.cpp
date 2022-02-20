#include "hooks.hpp"
#include "../../dependencies/ImGui/imgui_impl_dx9.h"

long __stdcall hooks::reset::hooked(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto hr = original(device, params);

	if (SUCCEEDED(hr))
		ImGui_ImplDX9_CreateDeviceObjects();

	return hr;
}