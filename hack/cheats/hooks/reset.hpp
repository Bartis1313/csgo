#pragma once

#include "helpers/common.hpp"

#include <d3d9.h>

_HOOK_BEGIN
class Reset
{
	ADD_HOOK_FRIEND;
private:
	using value = HRESULT;
	using call = value(D3DAPI*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);

	static value D3DAPI hook(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params);
	inline static call original;
};
_HOOK_END