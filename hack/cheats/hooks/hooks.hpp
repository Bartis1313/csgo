#pragma once

enum hookIndexes
{
	PAINTTRAVERSE = 41,
	CREATEMOVE = 24,
	DRAWMODEL_EXECUTE = 21,
	DRAWMODEL = 29,
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
	SET_DRAW_COLOR = 15, // overloaded
	RENDER_SMOKE_OVERLAY = 41,
	FIND_MATERIAL = 84,
	GET_VCOLLIDE = 6, // by index
	VCOLLIDE_LOAD = 36,
};

#define DUMP_WEATHER false

namespace hooks
{
	void init();
	void shutdown();

	// if you lazy to find every pattern, and know your thisptr is some sort of interface or your wanted class, store those here
	namespace capture
	{
		inline void* engineRenderer{ };
		inline void* particleEffects{ };
	}

	inline bool calledInitprecip = false;
}