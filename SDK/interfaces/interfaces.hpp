#pragma once

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
#pragma endregion

namespace interfaces
{
	inline ClientMode* clientMode = nullptr;
	inline IPanel* panel = nullptr;
	inline ISurface* surface = nullptr;
	inline IVEngineClient* engine = nullptr;
	inline IBaseClientDLL* client = nullptr;
	inline IWeapon* weapon = nullptr;
	inline IClientEntityList* entList = nullptr;
	inline Input* input = nullptr;
	inline CGlobalVarsBase* globalVars = nullptr;
	inline IEngineTrace* trace = nullptr;
	inline IMaterialSystem* matSys = nullptr;
	inline IVModelInfo* modelInfo = nullptr;
	inline IVDebugOverlay* debugOverlay = nullptr;
	inline IGameEventManager* eventManager = nullptr;
	inline IVRenderView* renderView = nullptr;
	inline IViewRender* viewRender = nullptr;
	inline CGlowManager* glowManager = nullptr;
	inline CGameMovement* gameMovement = nullptr;
	inline IPrediction* prediction = nullptr;
	inline IMoveHelper* moveHelper = nullptr;
	inline ILocalize* localize = nullptr;
	inline ICvar* cvar = nullptr;
	inline IConVar* convar = nullptr;
	inline IViewRenderBeams* beams = nullptr;
	inline IVModelRender* modelRender = nullptr;
	inline IVStudioRender* studioRender = nullptr;
	// remember to dereference!
	inline PlayerResource** resource = nullptr;
	inline IVEfx* efx = nullptr;
	inline InputSystem* iSystem = nullptr;
	inline IDirect3DDevice9* dx9Device = nullptr;
	inline IEffects* effects = nullptr;
	inline KeyValuesSys* keyValuesSys = nullptr;
	inline IMemAlloc* memAlloc = nullptr;
	inline IClientState* clientState = nullptr;
	inline CGameRules* gameRules = nullptr;
	inline IFileSystem* fileSystem = nullptr;

	bool init();

	inline bool done = false;
}

namespace gameFunctions
{	
	using tfn = void(__thiscall*)(CTeslaInfo&);
	inline tfn tesla = nullptr;
	using dfn = int(__fastcall*)(const char* name, const CEffectData&);
	inline dfn dispatchEffect = nullptr;
	// it's usercall
	//using dfnp = void(__thiscall*)(const char* particleName, const Vector&, const Vector&, void*, int);
}
