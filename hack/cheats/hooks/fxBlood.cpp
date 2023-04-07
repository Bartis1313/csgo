#include "hooks.hpp"

// 55 8B EC 83 EC 48 53 8B DA - color arr
//DecalModulate
//{
//$basetexture "Decals/Blood1"
//$decal 1
//$decalscale 0.15
//$vertexcolor 1
//$vertexalpha 1
//$splatter 1
//$fogfadeend 0.5
//}

//hooks::fxBlood::value FASTCALL hooks::fxBlood::hooked(FAST_ARGS, Vec3& pos, Vec3& dir, float r, float g, float b, float a)
//{
//	printf("called norm\n");
//	return original(thisptr, pos, dir, 0, 100, 200, 255);
//}
//
//hooks::fxBloodSpray::value FASTCALL hooks::fxBloodSpray::hooked(FAST_ARGS, const Vec3& pos, const Vec3& dir, float scale, uint8_t r, uint8_t g, uint8_t b, int flags)
//{
//	printf("called spray\n");
//	return original(thisptr, pos, dir, 0, 100, 200, 255, flags);
//}
//
//hooks::bloodUtilCallbackFn::value FASTCALL hooks::bloodUtilCallbackFn::hooked(FAST_ARGS, void* trace, int color)
//{
//	printf("Color %i\n", color);
//
//	original(thisptr, trace, color);
//}