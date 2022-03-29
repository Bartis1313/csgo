#include "interfaces.hpp"

#include <Windows.h>
#include <type_traits>
#include <d3d9.h>
#include <format>

#include "../structs/IDXandPaterrns.hpp"
#include "../../utilities/utilities.hpp"
#include "../../utilities/utilities.hpp"
#include "../../utilities/console/console.hpp"

template <typename T>
static T* getInterface(const std::string& moduleName, const std::string& interfaceName)
{
	// using fun = void* (*)(const char*, int*);
	using fun = void* __cdecl(const char*, int*);
	/*const auto capture = reinterpret_cast<fun>(
		LF(GetProcAddress)(LF(GetModuleHandleA)(moduleName), XOR("CreateInterface"))
		);*/
	const auto capture = reinterpret_cast<std::add_pointer_t<fun>>(
		LF(GetProcAddress)(LF(GetModuleHandleA)(moduleName.c_str()), XOR("CreateInterface"))
		);
	if (const auto ret = capture(interfaceName.c_str(), nullptr); ret != nullptr)
		return reinterpret_cast<T*>(ret);
	else
		LF(MessageBoxA)(nullptr, std::format(XOR("Interface {} was nullptr"), interfaceName).c_str(), XOR("Bartis hack"), MB_OK | MB_ICONERROR);

	return nullptr;
}

void interfaces::init()
{
	engine = getInterface<IVEngineClient>(ENGINE_DLL, XOR("VEngineClient014"));
	panel = getInterface<IPanel>(VGUI_DLL, XOR("VGUI_Panel009"));
	surface = getInterface<ISurface>(VGUIMAT_DLL, XOR("VGUI_Surface031"));
	client = getInterface<IBaseClientDLL>(CLIENT_DLL, XOR("VClient018"));
	entList = getInterface<IClientEntityList>(CLIENT_DLL, XOR("VClientEntityList003"));
	cvar = getInterface<ICvar>(VSTD_DLL, XOR("VEngineCvar007"));
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
	beams = *reinterpret_cast<IViewRenderBeams**>(utilities::patternScan(CLIENT_DLL, BEAMS, 0x1));
	input = *reinterpret_cast<Input**>((*reinterpret_cast<uintptr_t**>(client))[16] + 0x1);
	glowManager = *reinterpret_cast<CGlowManager**>(utilities::patternScan(CLIENT_DLL, GLOWMANAGER, 0x3));
	weapon = *reinterpret_cast<IWeapon**>(utilities::patternScan(CLIENT_DLL, WEAPONDATA, 0x2));
	moveHelper = **reinterpret_cast<IMoveHelper***>(utilities::patternScan(CLIENT_DLL, MOVEHELPER, 0x2));
	resource = *reinterpret_cast<PlayerResource***>(utilities::patternScan(CLIENT_DLL, PLAYER_RESOURCE, 0x2));
	dx9Device = **reinterpret_cast<IDirect3DDevice9***>(utilities::patternScan(SHARED_API, DX9_DEVICE, 0x1));

	console.log(TypeLogs::LOG_INFO, XOR("interfaces success"));
}
