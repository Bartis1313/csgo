#pragma once

#include "ifc.hpp"

#pragma region declarations
class IPanel;
class ClientMode;
class ISurface;
class IVEngineClient;
class IBaseClientDLL;
class IWeapon;
class IClientEntityList;
class IClientMode;
class CClientState;
class Input;
class CGlobalVarsBase;
class IEngineTrace;
class IMaterialSystem;
class IVModelInfo;
class IVDebugOverlay;
class IGameEventManager;
class IViewRender;
class IVRenderView;
class CGlowManager;
class CGameMovement;
class IPrediction;
class IMoveHelper;
class ILocalize;
class ICvar;
class IConVar;
class IViewRenderBeams;
class IVModelRender;
class IVStudioRender;
class PlayerResource;
class IVEfx;
class InputSystem;
struct IDirect3DDevice9;
class IEffects;
class KeyValuesSys;
class IMemAlloc;
class CTeslaInfo;
class CEffectData;
class IClientState;
class Entity_t;
class IFileSystem;
class CGameRules;
class CEngineSoundClient;
class IMDLCache;
class IBaseFileSystem;
#pragma endregion

namespace memory::interfaces
{
	inline Interface<IVEngineClient*> engine;
	inline Interface<IPanel*> panel;
	inline Interface<ISurface*> surface;
	inline Interface<IBaseClientDLL*> client;
	inline Interface<IClientEntityList*> entList;
	inline Interface<ICvar*> cvar;
	inline Interface<IEngineTrace*> trace;
	inline Interface<IVRenderView*> renderView;
	inline Interface<IMaterialSystem*> matSys;
	inline Interface<IVModelInfo*> modelInfo;
	inline Interface<IPrediction*> prediction;
	inline Interface<CGameMovement*> gameMovement;
	inline Interface<IVDebugOverlay*> debugOverlay;
	inline Interface<ILocalize*> localize;
	inline Interface<IVModelRender*> modelRender;
	inline Interface<IVStudioRender*> studioRender;
	inline Interface<IGameEventManager*> eventManager;
	inline Interface<IVEfx*> efx;
	inline Interface<InputSystem*> iSystem;
	inline Interface<IEffects*> effects;
	inline Interface<IFileSystem*> fileSystem;
	inline Interface<CEngineSoundClient*> sound;
	inline Interface<IMDLCache*> mdlCache;
	inline Interface<IBaseFileSystem*> baseFileSystem;

	void init();
}