#pragma once
#include "../../SDK/interfaces/interfaces.hpp"

enum hookIndexes
{
	PAINTTRAVERSE = 41,
	CREATEMOVE = 24,
	DRAWMODEL = 21,
	OVERRIDE = 18,
	POSTSCREENEFFECT = 44,
	FRAMESTAGE = 37,
	//LOCK_CURSOR = 67, why another hook
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
}