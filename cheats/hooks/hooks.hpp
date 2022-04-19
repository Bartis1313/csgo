#pragma once

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
	SV_CHEATS = 13
};

class IPanel;
class CViewSetup;
struct DrawModelState_t;
struct ModelRenderInfo_t;
struct Matrix3x4;
struct CStudioHdr;
class CParticleCollection;

#define FAST_ARGS void* thisptr, void* edx

namespace hooks
{
	void init();
	void shutdown();

	struct paintTraverse
	{
		using fn = void(__thiscall*)(IPanel*, unsigned int, bool, bool);
		static void __stdcall hooked(unsigned int panel, bool forceRepaint, bool allowForce);
		inline static fn original = nullptr;
		static const int index = PAINTTRAVERSE;
	};

	/*struct createMove
	{
		using fn = bool(__stdcall*)(float, CUserCmd*);
		static bool __stdcall hooked(float inputFrame, CUserCmd* cmd);
		inline static fn original = nullptr;
		static const int index = CREATEMOVE;
	};*/

	struct drawModel
	{
		using fn = void(__thiscall*)(void*, void*, const DrawModelState_t&, const ModelRenderInfo_t&, Matrix3x4*);
		static void __stdcall hooked(void* result, const DrawModelState_t& state, const ModelRenderInfo_t& info, Matrix3x4* matrix);
		inline static fn original = nullptr;
		static const int index = DRAWMODEL;
	};

	struct overrideView
	{
		using fn = void(__stdcall*)(CViewSetup*);
		static void __stdcall hooked(CViewSetup* view);
		inline static fn original = nullptr;
		static const int index = OVERRIDE;
	};

	struct doPostScreenEffects
	{
		using fn = int(__thiscall*)(void*, int);
		static int __stdcall hooked(int val);
		inline static fn original = nullptr;
		static const int index = POSTSCREENEFFECT;
	};

	struct frameStageNotify
	{
		using fn = void(__thiscall*)(void*, int);
		static void __stdcall hooked(int frame);
		inline static fn original = nullptr;
		static const int index = FRAMESTAGE;
	};	

	struct clientValidAddr
	{
		using fn = char(__thiscall*)(void*, const char*);
		static char __fastcall hooked(FAST_ARGS, const char* lpModuleName);
		inline static fn original = nullptr;
	};

	struct engineValidAddr
	{
		using fn = char(__thiscall*)(void*, const char*);
		static char __fastcall hooked(FAST_ARGS, const char* lpModuleName);
		inline static fn original = nullptr;
	};

	struct studioRenderValidAddr
	{
		using fn = char(__thiscall*)(void*, const char*);
		static char __fastcall hooked(FAST_ARGS, const char* lpModuleName);
		inline static fn original = nullptr;
	};

	struct materialSystemValidAddr
	{
		using fn = char(__thiscall*)(void*, const char*);
		static char __fastcall hooked(FAST_ARGS, const char* lpModuleName);
		inline static fn original = nullptr;
	};

	struct isUsingStaticPropDebugModes
	{
		using fn = bool(__thiscall*)(void*);
		static bool __fastcall hooked(FAST_ARGS);
		inline static fn original = nullptr;
	};

	struct lockCursor
	{
		using fn = void(__thiscall*)(void*);
		static void __stdcall hooked();
		inline static fn original = nullptr;
		static const int index = LOCK_CURSOR;
	};

	struct reset
	{
		using fn = long(__stdcall*)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*);
		static long __stdcall hooked(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params);
		inline static fn original = nullptr;
		static const int index = RESETDX;
	};

	struct present
	{
		using fn = long(__stdcall*)(IDirect3DDevice9*, RECT*, RECT*, HWND, RGNDATA*);
		static long __stdcall hooked(IDirect3DDevice9* device, RECT* srcRect, RECT* dstRect, HWND window, RGNDATA* region);
		inline static fn original = nullptr;
		static const int index = PRESENTDX;
	};

	/*struct endScene
	{
		using fn = long(__stdcall*)(IDirect3DDevice9*);
		static long __stdcall hooked(IDirect3DDevice9* device);
		inline static fn original;
		static const int index = END_SCENE;
	};*/

	struct wndProcSys
	{
		inline static WNDPROC wndProcOriginal = nullptr;
		inline static HWND currentWindow = nullptr;
		inline static LRESULT __stdcall wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

		static void init();
		static void shutdown();
	};

	struct proxyCreateMove
	{
		using fn = void(__stdcall*)(int, float, bool);
		static void __stdcall hooked(int sequence, float inputFrame, bool active);
		inline static fn original = nullptr;
		static const int index = PROXY_MOVE;
	};

	struct getColorModulation
	{
		using fn = void(__thiscall*)(void*, float*, float*, float*);
		static void __fastcall hooked(FAST_ARGS, float* r, float* g, float* b);
		inline static fn original = nullptr;
	};

	struct setupBones
	{
		using fn = bool(__thiscall*)(void*, Matrix3x4*, int, int, float);
		static bool __fastcall hooked(FAST_ARGS, Matrix3x4*, int maxBones, int boneMask, float curtime);
		inline static fn original = nullptr;
	};

	struct buildTransformations
	{
		using fn = void(__thiscall*)(void*, CStudioHdr*, void*, void*, const Matrix3x4&, int, void*);
		static void __fastcall hooked(FAST_ARGS, CStudioHdr* hdr, void* pos, void* q, const Matrix3x4& matrix, int boneMask, void* computed);
		inline static fn original = nullptr;
	};

	struct doExtraBonesProcessing
	{
		using fn = void(__thiscall*)(void*, void*, void*, const Matrix3x4&, void*, void*);
		static void __fastcall hooked(FAST_ARGS, void* hdr, void* pos, void* q, const Matrix3x4& matrix, void* computed, void* contex);
		inline static fn original = nullptr;
	};

	struct sv_cheats
	{
		using fn = bool(__thiscall*)(void*);
		static bool __fastcall hooked(FAST_ARGS);
		inline static fn original = nullptr;
		static const int index = SV_CHEATS;
	};

	struct particlesSimulations
	{
		using fn = void(__thiscall*)(CParticleCollection*);
		static void __fastcall hooked(CParticleCollection* thisPtr, void* edx);
		inline static fn original = nullptr;
	};
}