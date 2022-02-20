#pragma once
#include "../../SDK/interfaces/interfaces.hpp"

#define FASTARGS void* thisptr, void* edx

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
};

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

	struct createMove
	{
		using fn = bool(__stdcall*)(float, CUserCmd*);
		static bool __stdcall hooked(float inputFrame, CUserCmd* cmd);
		inline static fn original = nullptr;
		static const int index = CREATEMOVE;
	};

	struct drawModel
	{
		using fn = void(__thiscall*)(void*, void*, const DrawModelState_t&, const ModelRenderInfo_t&, matrix3x4_t*);
		static void __stdcall hooked(void* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix);
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
		using fn = char(__fastcall*)(void*, void*, const char*);
		static char __fastcall hooked(void* thisptr, void* edx, const char* lpModuleName);
		inline static fn original = nullptr;
	};

	struct engineValidAddr
	{
		using fn = char(__fastcall*)(void*, void*, const char*);
		static char __fastcall hooked(void* thisptr, void* edx, const char* lpModuleName);
		inline static fn original = nullptr;
	};

	struct studioRenderValidAddr
	{
		using fn = char(__fastcall*)(void*, void*, const char*);
		static char __fastcall hooked(void* thisptr, void* edx, const char* lpModuleName);
		inline static fn original = nullptr;
	};

	struct materialSystemValidAddr
	{
		using fn = char(__fastcall*)(void*, void*, const char*);
		static char __fastcall hooked(void* thisptr, void* edx, const char* lpModuleName);
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
		using fn = void(__fastcall*)(void*, int, int, float, bool);
		static void __fastcall hooked(void*, int, int sequence, float inputTime, bool active);
		inline static fn original = nullptr;
		static const int index = PROXY_MOVE;
	};
}