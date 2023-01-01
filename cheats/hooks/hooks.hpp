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
	UNKOWN_FILESYS = 89,
	UNVERIFIED_FILE_HASHES= 101,
	UNK_FILE_CHECK = 128,
	RENDER_VIEW = 6,
	IS_HLTV = 93,
	SCREEN_2D_EFFECT = 39,
	LEVEL_INIT_PREENT = 5,
	LEVEL_INIT_POSTENT = 6,
	LEVEL_SHUTDOWN = 7,
	EMIT_SOUND = 5,
	CREATE_EVENT = 7,
	RUN_COMMAND = 19,
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

#define FAST_ARGS [[maybe_unused]] void* thisptr, [[maybe_unused]] void* edx
#define FASTCALL __fastcall
// for easier macro in cpp
#define EMPTY_INDEX	0xFFF

#define HOOK_STRUCT_VFUNC(name, type, idx, ...) \
struct name { \
	using value = type; \
	using fn = type(__thiscall*)(void*, __VA_ARGS__); \
	static type __fastcall hooked(FAST_ARGS, __VA_ARGS__); \
	inline static fn original = nullptr; \
	static constexpr int index = idx; \
};

#define HOOK_STRUCT_FUNC(name, type, ...) \
struct name { \
	using value = type; \
	using fn = type(__thiscall*)(void*, __VA_ARGS__); \
	static type __fastcall hooked(FAST_ARGS, __VA_ARGS__); \
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

namespace hooks
{
	void init();
	void shutdown();

	inline static constexpr auto indexNone = EMPTY_INDEX;

	HOOK_STRUCT_VFUNC(paintTraverse, void, PAINTTRAVERSE, uint32_t, bool, bool);
	HOOK_STRUCT_VFUNC(drawModel, void, DRAWMODEL, void*, const DrawModelState_t&, const ModelRenderInfo_t&, Matrix3x4*);
	HOOK_STRUCT_VFUNC(overrideView, void, OVERRIDE, CViewSetup*);
	HOOK_STRUCT_VFUNC(doPostScreenEffects, int, POSTSCREENEFFECT, int);
	HOOK_STRUCT_VFUNC(frameStageNotify, void, FRAMESTAGE, int);
	HOOK_STRUCT_VFUNC(lockCursor, void, LOCK_CURSOR);
	HOOK_STRUCT_VFUNC(proxyCreateMove, void, PROXY_MOVE, int, float, bool);
	HOOK_STRUCT_VFUNC(unknownFileSystem, int, UNKOWN_FILESYS, void*);
	HOOK_STRUCT_VFUNC(getUnverifiedFileHashes, int, UNVERIFIED_FILE_HASHES, int);
	HOOK_STRUCT_VFUNC(unkFileCheck, int, UNK_FILE_CHECK);
	HOOK_STRUCT_VFUNC(sv_cheats, bool, SV_CHEATS);
	HOOK_STRUCT_VFUNC(renderView, void, RENDER_VIEW, const CViewSetup&, const CViewSetup&, int, int);
	HOOK_STRUCT_VFUNC(isHltv, bool, IS_HLTV);
	HOOK_STRUCT_VFUNC(screen2DEffect, void, SCREEN_2D_EFFECT, CViewSetup*);
	// reset everything
	HOOK_STRUCT_VFUNC(levelInitPreEntity, void, LEVEL_INIT_PREENT, const char*);
	// map loads - use any visual change
	HOOK_STRUCT_VFUNC(levelInitPostEntity, void, LEVEL_INIT_POSTENT);
	HOOK_STRUCT_VFUNC(levelShutdown, void, LEVEL_SHUTDOWN);
	HOOK_STRUCT_VFUNC(emitSound, void, EMIT_SOUND, void*, int, int, const char*, uint32_t, const char*, float, int, int, int, int, const Vec3&, const Vec3&, void*, bool, float, int, void*);
	HOOK_STRUCT_VFUNC(createEvent, IGameEvent*, CREATE_EVENT, const char*, bool, uint32_t);
	HOOK_STRUCT_VFUNC(runCommand, void, RUN_COMMAND, Player_t*, CUserCmd*, IMoveHelper*);

	HOOK_STRUCT_FUNC(getColorModulation, void, float*, float*, float*);
	HOOK_STRUCT_FUNC(buildTransformations, void, CStudioHdr*, void*, void*, const Matrix3x4&, int, void*);
	HOOK_STRUCT_FUNC(doExtraBonesProcessing, void, void*, void*, void* q, const Matrix3x4&, void*, void*);
	HOOK_STRUCT_FUNC(particlesSimulations, void);
	HOOK_STRUCT_FUNC(sendDatagram, int, void*);
	HOOK_STRUCT_FUNC(unknownOverViewFun, int, int);
	HOOK_STRUCT_FUNC(isDepthOfField, bool);
	HOOK_STRUCT_FUNC(fxBlood, void, Vec3&, Vec3&, float, float, float, float);
	HOOK_STRUCT_FUNC(clientValidAddr, char, const char*);
	HOOK_STRUCT_FUNC(engineValidAddr, char, const char*);
	HOOK_STRUCT_FUNC(studioRenderValidAddr, char, const char*);
	HOOK_STRUCT_FUNC(materialSystemValidAddr, char, const char*);
	HOOK_STRUCT_FUNC(isUsingStaticPropDebugModes, bool);
	HOOK_STRUCT_FUNC(addEnt, void, void*, EHandle_t);
	HOOK_STRUCT_FUNC(removeEnt, void, void*, EHandle_t);
	HOOK_STRUCT_FUNC(isFollowingEntity, bool);
	HOOK_STRUCT_FUNC(processSpottedEntityUpdate, bool, void*);
	HOOK_STRUCT_FUNC(fireIntern, void, IGameEvent*, bool, bool);
	HOOK_STRUCT_FUNC(preRestartRound, void);
	HOOK_STRUCT_FUNC(playStepSound, int, Vec3&, void*, float, bool, void*);

	HOOK_STRUCT_API_VFUNC(reset, long, RESETDX, IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
	HOOK_STRUCT_API_VFUNC(present, long, PRESENTDX, IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA*);
	HOOK_STRUCT_API_VFUNC(drawIndexedPrimitive, long, DRAW_IDX_PRIMITIVE, IDirect3DDevice9*, D3DPRIMITIVETYPE, INT, UINT, UINT, UINT, UINT);
}

#undef HOOK_STRUCT_VFUNC
#undef HOOK_STRUCT_FUNC
#undef HOOK_STRUCT_API_VFUNC
#undef HOOK_STRUCT_API
#undef EMPTY_INDEX
