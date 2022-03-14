#include "interfaces.hpp"
#include "../structs/IDXandPaterrns.hpp"
#include "../../utilities/utilities.hpp"
#include "../../cheats/globals.hpp"

void interfaces::init()
{
	engine = getInterface<IVEngineClient>(ENGINE_DLL, XOR("VEngineClient014"));
	panel = getInterface<IPanel>(VGUI_DLL, XOR("VGUI_Panel009"));
	surface = getInterface<ISurface>(VGUIMAT_DLL, XOR("VGUI_Surface031"));
	client = getInterface<IBaseClientDLL>(CLIENT_DLL, XOR("VClient018"));
	entList = getInterface<IClientEntityList>(CLIENT_DLL, XOR("VClientEntityList003"));
	console = getInterface<ICvar>(VSTD_DLL, XOR("VEngineCvar007"));
	trace = getInterface<IEngineTrace>(ENGINE_DLL, XOR("EngineTraceClient004"));
	renderView = getInterface<IViewRender>(ENGINE_DLL, XOR("VEngineRenderView014"));
	matSys = getInterface<IMaterialSystem>(MATERIAL_DLL, XOR("VMaterialSystem080"));
	modelInfo = getInterface<IVModelInfo>(ENGINE_DLL, XOR("VModelInfoClient004"));
	prediction = getInterface<IPrediction>(CLIENT_DLL, XOR("VClientPrediction001"));
	gameMovement = getInterface<CGameMovement>(CLIENT_DLL, XOR("GameMovement001"));
	debugOverlay = getInterface<IVDebugOverlay>(ENGINE_DLL, XOR("VDebugOverlay004"));
	localize = getInterface<ILocalize>(LOCALIZE_DLL, XOR("Localize_001"));
	modelRender = getInterface<IVModelRender>(ENGINE_DLL, XOR("VEngineModel016"));
	studioRender = getInterface<IVStudioRender>(STUDIORENDER_DLL, XOR("VStudioRender026"));
	eventManager = getInterface<IGameEventManager>(ENGINE_DLL, XOR("GAMEEVENTSMANAGER002"));
	efx = getInterface<IVEfx>(ENGINE_DLL, XOR("VEngineEffects001"));
	iSystem = getInterface<InputSystem>(INPUTSYSTEM_DLL, XOR("InputSystemVersion001"));
	effects = getInterface<IEffects>(CLIENT_DLL, XOR("IEffects001"));
	
	globalVars = **reinterpret_cast<CGlobalVarsBase***>((*reinterpret_cast<uintptr_t**>(client))[0] + 0x1F);
	clientMode = **reinterpret_cast<ClientMode***>((*reinterpret_cast<uintptr_t**>(client))[10] + 0x5);
	beams = *reinterpret_cast<IViewRenderBeams**>(utilities::patternScan(CLIENT_DLL, BEAMS) + 0x1);
	glowManager = reinterpret_cast<CGlowManager*>(*reinterpret_cast<uintptr_t*>(utilities::patternScan(CLIENT_DLL, GLOWMANAGER) + 0x3));
	weapon = *reinterpret_cast<IWeapon**>(utilities::patternScan(CLIENT_DLL, WEAPONDATA) + 0x2);
	moveHelper = **reinterpret_cast<IMoveHelper***>(utilities::patternScan(CLIENT_DLL, MOVEHELPER) + 0x2);
	input = *reinterpret_cast<Input**>((*reinterpret_cast<uintptr_t**>(client))[16] + 0x1);
	resource = *reinterpret_cast<PlayerResource***>(utilities::patternScan(CLIENT_DLL, PLAYER_RESOURCE) + 0x2);
	dx9Device = **reinterpret_cast<IDirect3DDevice9***>(utilities::patternScan(SHARED_API, DX9_DEVICE) + 0x1);

	globals::interfacesDone = true;

	// because it's first log ever
	interfaces::engine->executeClientCMD(XOR("clear"));

	LOG(LOG_INFO, XOR("interfaces success"));
}
