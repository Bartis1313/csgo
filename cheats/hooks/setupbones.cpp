#include "hooks.hpp"
#include "../../SDK/structs/Entity.hpp"

// this would need some rebuild if you experience a lot of drops with some features requiring bones every frame
// but it has to be 100% correct, this is very important function, or mess witrh extra processing? I don't honeslty know. I will patch this in short future
//bool __fastcall hooks::setupBones::hooked(FAST_ARGS, Matrix3x4*, int maxBones, int boneMask, float curtime)
//{
//
//}