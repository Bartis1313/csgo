#pragma once

#include "helpers/common.hpp"

#include <SDK/CCommonHostState.hpp>

_HOOK_BEGIN
class R_AddDecalToSurface
{
	ADD_HOOK_FRIEND;
private:
	using value = void;
	using call = value(STDCALL*)(decal_t*, SurfaceHandle_t, decalinfo_t*);

	static value STDCALL hook(decal_t* pdecal, SurfaceHandle_t surfID, decalinfo_t* decalinfo);
	inline static call original;
};
_HOOK_END