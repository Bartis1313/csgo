#pragma once

#include <SDK/math/matrix.hpp>
#include <SDK/math/Vector.hpp>

#include <d3d9.h>

enum hookIndexes
{
	PAINTTRAVERSE = 41,
	CREATEMOVE = 24,
	DRAWMODEL = 21,
	OVERRIDE = 18,
	POSTSCREENEFFECT = 44,
	FRAMESTAGE = 37,
	LOCK_CURSOR = 67,
	RESETDX = 16,
	PRESENTDX = 17,
	PROXY_MOVE = 22,
	END_SCENE = 42,
	SV_CHEATS = 13,
	DRAW_IDX_PRIMITIVE = 82,
	UNK_ALLOC_FILESYS = 89, // probably some from the IO operation method used by textures for optimal use
	UNVERIFIED_FILE_HASHES= 101,
	LOSE_FILES_CHECK = 128, // 512 / 4
	RENDER_VIEW = 6,
	IS_HLTV = 93,
	SCREEN_2D_EFFECT = 39,
	LEVEL_INIT_PREENT = 5,
	LEVEL_INIT_POSTENT = 6,
	LEVEL_SHUTDOWN = 7,
	EMIT_SOUND = 5,
	CREATE_EVENT = 7,
	RUN_COMMAND = 19,
	RENDER_VIEW_SCENE_END = 9,
	OVERRIDE_MOUSE = 23,
	ALLOC_KEYVALUE = 2,
};

class IPanel;
class CViewSetup;
struct DrawModelState_t;
struct ModelRenderInfo_t;
struct CStudioHdr;
class CParticleCollection;
class INetChannel;
struct MapStruct;
class IMaterialSystem;
class IGameEvent;
class CUserCmd;
class Player_t;
class IMoveHelper;
struct EHandle_t;
class IMaterial;

#define FAST_ARGS [[maybe_unused]] void* thisptr, [[maybe_unused]] void* edx
#define FASTCALL __fastcall
// for easier macro in cpp
#define EMPTY_INDEX	0xFFF

// we have index of hooked CLASS member func
#define HOOK_STRUCT_VFUNC_IDX(name, type, idx, ...) \
struct name { \
	using value = type; \
	using fn = type(__thiscall*)(void*, __VA_ARGS__); \
	static type __fastcall hooked(FAST_ARGS, __VA_ARGS__); \
	inline static fn original = nullptr; \
	static constexpr int index = idx; \
};

// we have addr of hooked CLASS member func
#define HOOK_STRUCT_VFUNC_ADDR(name, type, ...) \
struct name { \
	using value = type; \
	using fn = type(__thiscall*)(void*, __VA_ARGS__); \
	static type __fastcall hooked(FAST_ARGS, __VA_ARGS__); \
	inline static fn original = nullptr; \
	static constexpr int index = EMPTY_INDEX; \
};

// we have addr of hooked func
#define HOOK_STRUCT_FUNC(name, type, ...) \
struct name { \
	using value = type; \
	using fn = type(__stdcall*)(__VA_ARGS__); \
	static type __stdcall hooked(__VA_ARGS__); \
	inline static fn original = nullptr; \
	static constexpr int index = EMPTY_INDEX; \
};

#define HOOK_STRUCT_API_VFUNC(name, type, idx, ...) \
struct name { \
	using value = type; \
	using fn = type(__stdcall*)(__VA_ARGS__); \
	static type __stdcall hooked(__VA_ARGS__); \
	inline static fn original = nullptr; \
	static constexpr int index = idx; \
};

#define HOOK_STRUCT_API(name, type, ...) \
struct name { \
	using value = type; \
	using fn = type(__stdcall*)(__VA_ARGS__); \
	static type __stdcall hooked(__VA_ARGS__); \
	inline static fn original = nullptr; \
	static constexpr int index = EMPTY_INDEX; \
};

#define DEFINE_HOOK_CLASS_MEMBER(name, ...) \
name::value name::hooked(FAST_ARGS, __VA_ARGS__)

#define DEFINE_HOOK_FUNC(name, ...) \
name::value name::hooked(__VA_ARGS__)

namespace hooks
{
	void init();
	void shutdown();

	inline static constexpr auto indexNone = EMPTY_INDEX;

	HOOK_STRUCT_VFUNC_IDX(paintTraverse, void, PAINTTRAVERSE, uint32_t, bool, bool);
	HOOK_STRUCT_VFUNC_IDX(drawModel, void, DRAWMODEL, void*, const DrawModelState_t&, const ModelRenderInfo_t&, Matrix3x4*);
	HOOK_STRUCT_VFUNC_IDX(overrideView, void, OVERRIDE, CViewSetup*);
	HOOK_STRUCT_VFUNC_IDX(doPostScreenEffects, int, POSTSCREENEFFECT, int);
	HOOK_STRUCT_VFUNC_IDX(frameStageNotify, void, FRAMESTAGE, int);
	HOOK_STRUCT_VFUNC_IDX(lockCursor, void, LOCK_CURSOR);
	HOOK_STRUCT_VFUNC_IDX(createMove, bool, CREATEMOVE, float, CUserCmd*);
	HOOK_STRUCT_VFUNC_IDX(unknownFileSystemAlloc, int, UNK_ALLOC_FILESYS, void*);
	HOOK_STRUCT_VFUNC_IDX(getUnverifiedFileHashes, int, UNVERIFIED_FILE_HASHES, int);
	HOOK_STRUCT_VFUNC_IDX(filesCheck, int, LOSE_FILES_CHECK);
	HOOK_STRUCT_VFUNC_IDX(sv_cheats, bool, SV_CHEATS);
	HOOK_STRUCT_VFUNC_IDX(viewRender, void, RENDER_VIEW, const CViewSetup&, const CViewSetup&, int, int);
	HOOK_STRUCT_VFUNC_IDX(isHltv, bool, IS_HLTV);
	HOOK_STRUCT_VFUNC_IDX(screen2DEffect, void, SCREEN_2D_EFFECT, CViewSetup*);
	// reset everything
	HOOK_STRUCT_VFUNC_IDX(levelInitPreEntity, void, LEVEL_INIT_PREENT, const char*);
	// map loads - use any visual change
	HOOK_STRUCT_VFUNC_IDX(levelInitPostEntity, void, LEVEL_INIT_POSTENT);
	HOOK_STRUCT_VFUNC_IDX(levelShutdown, void, LEVEL_SHUTDOWN);
	HOOK_STRUCT_VFUNC_IDX(emitSound, void, EMIT_SOUND, void*, int, int, const char*, uint32_t, const char*, float, int, int, int, int, const Vec3&, const Vec3&, void*, bool, float, int, void*);
	HOOK_STRUCT_VFUNC_IDX(createEvent, IGameEvent*, CREATE_EVENT, const char*, bool, uint32_t);
	HOOK_STRUCT_VFUNC_IDX(runCommand, void, RUN_COMMAND, Player_t*, CUserCmd*, IMoveHelper*);
	HOOK_STRUCT_VFUNC_IDX(sceneEnd, void, RENDER_VIEW_SCENE_END);
	HOOK_STRUCT_VFUNC_IDX(overrideMouse, void, OVERRIDE_MOUSE, float*, float*);
	HOOK_STRUCT_VFUNC_IDX(allocKeyValues, void*, ALLOC_KEYVALUE, size_t);

	HOOK_STRUCT_VFUNC_ADDR(getColorModulation, void, float*, float*, float*);
	HOOK_STRUCT_VFUNC_ADDR(buildTransformations, void, CStudioHdr*, void*, void*, const Matrix3x4&, int, void*);
	HOOK_STRUCT_VFUNC_ADDR(doExtraBonesProcessing, void, void*, void*, void* q, const Matrix3x4&, void*, void*);
	HOOK_STRUCT_VFUNC_ADDR(particlesSimulations, void);
	HOOK_STRUCT_VFUNC_ADDR(sendDatagram, int, void*);
	HOOK_STRUCT_VFUNC_ADDR(unknownOverViewFun, int, int);
	HOOK_STRUCT_FUNC(isDepthOfField, bool);
	HOOK_STRUCT_FUNC(isUsingStaticPropDebugModes, bool);
	HOOK_STRUCT_VFUNC_ADDR(addEnt, void, void*, EHandle_t);
	HOOK_STRUCT_VFUNC_ADDR(removeEnt, void, void*, EHandle_t);
	HOOK_STRUCT_VFUNC_ADDR(isFollowingEntity, bool);
	HOOK_STRUCT_VFUNC_ADDR(processSpottedEntityUpdate, bool, void*);
	HOOK_STRUCT_VFUNC_ADDR(fireIntern, void, IGameEvent*, bool, bool);
	HOOK_STRUCT_VFUNC_ADDR(preRestartRound, void, void*);
	HOOK_STRUCT_VFUNC_ADDR(playStepSound, int, Vec3&, void*, float, bool, void*);
	HOOK_STRUCT_VFUNC_ADDR(chudIsHidden, bool, int);
	HOOK_STRUCT_VFUNC_ADDR(viewDrawFade, void, uint8_t*, IMaterial*, bool);
	HOOK_STRUCT_VFUNC_ADDR(unkround, void, void*);
	HOOK_STRUCT_API_VFUNC(reset, HRESULT, RESETDX, IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
	HOOK_STRUCT_API_VFUNC(present, HRESULT, PRESENTDX, IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA*);
	HOOK_STRUCT_API_VFUNC(drawIndexedPrimitive, HRESULT, DRAW_IDX_PRIMITIVE, IDirect3DDevice9*, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
}

#undef HOOK_STRUCT_VFUNC
#undef HOOK_STRUCT_VFUNC_IDX
#undef HOOK_STRUCT_VFUNC_ADDR
#undef HOOK_STRUCT_FUNC
#undef HOOK_STRUCT_API_VFUNC
#undef HOOK_STRUCT_API
#undef EMPTY_INDEX
